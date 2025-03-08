#include "start_up_wifi.h"


static const char *TAG = "http_server";

static void wifi_event_handler(void *arg,
			esp_event_base_t event_base,
			int32_t event_id,
			void *event_data) {

	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t*) event_data;
		ESP_LOGI(TAG,"station "MACSTR" join, AID=%d",
			MAC2STR(event->mac),
			event->aid);
	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t*) event_data;
		ESP_LOGI(TAG,"station "MACSTR" leave, AID=%d, reason=%d",
			MAC2STR(event->mac),
			event->aid,
			event->reason);
	}
}


void wifi_init_softap(void) {
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
								ESP_EVENT_ANY_ID,
								&wifi_event_handler,
								NULL,
								NULL));

	wifi_config_t wifi_config = {
		.ap = {
			.ssid = ESP_WIFI_SSID,
			.ssid_len = strlen(ESP_WIFI_SSID),
			.channel = ESP_WIFI_CHAN,
			.password = ESP_WIFI_PASS,
			.max_connection = ESP_MAX_STA,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
			.authmode = WIFI_AUTH_WPA3_PSK,
			.sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else
			.authmode = WIFI_AUTH_WPA2_PSK,
#endif
			.pmf_cfg = {
				.required = true
			},
		},
	};
	if (strlen(ESP_WIFI_PASS) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG,"wifi_init_softap finished. SSID:%s password:%s channel:%d",
		ESP_WIFI_SSID,ESP_WIFI_PASS,ESP_WIFI_CHAN);
}


#define MAXB 64
#define MAXR 2048

static char resp[MAXR];

static char control_0[MAXB] = "0000";
static char control_1[MAXB] = "0000";
static char control_2[MAXB] = "0000";
static char control_3[MAXB] = "0000";

static int err;

esp_err_t update_root_resp(){

	memset(resp,0,MAXR);
	err=sprintf(resp,"<!DOCTYPE html>"
			"<html>"
			"<head>"
				"<title>\"MASTER\"</title>"
			"</head>"
			"<body>"
				"<h1>\"MASTER\"</h1>"
				"<p>\"textbox test\"</p>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_0\">control_0: </label>"
					"<input type=\"number\" name=\"control_0\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_1\">control_1: </label>"
					"<input type=\"number\" name=\"control_1\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_2\">control_2: </label>"
					"<input type=\"number\" name=\"control_2\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_3\">control_3: </label>"
					"<input type=\"number\" name=\"control_3\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
			"</body>"
			"</html>",
			control_0,
			control_1,
			control_2,
			control_3);
	return err;
}

size_t header_field_len(httpd_req_t *req,const char *field){

	size_t len = httpd_req_get_hdr_value_len(req,field);
	switch(len){
		case 0:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_get_hdr_value_len: Length is zero");
			break;
		default:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_get_hdr_value_len: ESP_OK");
			printf("Length: %d",len);
			fflush(stdout);
			break;
	}
	return len;
}


int validate_body(httpd_req_t *req,char *bod_buf){

	switch (err=httpd_req_recv(req,bod_buf,MAXB)){
		case 0:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_recv: Length is zero");
			break;
		case HTTPD_SOCK_ERR_INVALID:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_recv: HTTPD_SOCK_ERR_INVALID");
			break;
		case HTTPD_SOCK_ERR_TIMEOUT:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_recv: HTTPD_SOCK_ERR_TIMEOUT");
			break;
		case HTTPD_SOCK_ERR_FAIL:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_recv: HTTPD_SOCK_ERR_FAIL");
			break;
		default:
			ESP_LOGI(TAG,"root_post_handler: httpd_req_recv: ESP_OK");
			printf("bytes written: %d\nbuffer value: %s\n",err, bod_buf);
			fflush(stdout);
			break;
	}
	return err;
}


esp_err_t validate_control(httpd_req_t *req,char *bod_buf,char *control_x,const char *control_x_str_lit,int x){

	char control_x_temp[MAXB];
	switch (err=httpd_query_key_value(bod_buf,control_x_str_lit,control_x_temp,MAXB)){
		case ESP_ERR_NOT_FOUND:
			ESP_LOGI(TAG,"root_post_handler: httpd_query_key_value: ESP_ERR_NOT_FOUND");
			break;
		case ESP_ERR_INVALID_ARG:
			ESP_LOGI(TAG,"root_post_handler: httpd_query_key_value: ESP_ERR_INVALID_ARG");
			break;
		case ESP_ERR_HTTPD_RESULT_TRUNC:
			ESP_LOGI(TAG,"root_post_handler: httpd_query_key_value: ESP_ERR_HTTPD_RESULT_TRUNC");
			break;
		case ESP_OK:
			ESP_LOGI(TAG,"root_post_handler: httpd_query_key_value: ESP_OK");
			memset(control_x,0,strlen(control_x));
			strcpy(control_x,control_x_temp);
			break;
	}
	printf("control_%d: %s\n",x,control_x);
	fflush(stdout);
	return err;
}


esp_err_t root_get_handler(httpd_req_t *req){
	httpd_resp_send(req,resp,HTTPD_RESP_USE_STRLEN);
	ESP_LOGI(TAG,"root_get_handler: sent response");
	return ESP_OK;
}


esp_err_t root_post_handler(httpd_req_t *req){
	char bod_buf[MAXB];
	validate_body(req,bod_buf);
	validate_control(req,bod_buf,control_0,"control_0",0);
	validate_control(req,bod_buf,control_1,"control_1",1);
	validate_control(req,bod_buf,control_2,"control_2",2);
	validate_control(req,bod_buf,control_3,"control_3",3);
	update_root_resp();
	err=httpd_resp_send(req,resp,HTTPD_RESP_USE_STRLEN);
	ESP_LOGI(TAG,"root_post_handler: sent response");
	return err;
}


esp_err_t fake_handler(httpd_req_t *req){return ESP_OK;}





httpd_uri_t root_get_uri = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = root_get_handler,
};

httpd_uri_t root_post_uri = {
	.uri = "/",
	.method = HTTP_POST,
	.handler = root_post_handler,
	.user_ctx = NULL
};

httpd_uri_t fake_uri = {
	.uri = "/favicon.ico",
	.method = HTTP_GET,
	.handler = fake_handler,
	.user_ctx = NULL
};




httpd_handle_t start_httpd_server(void) {

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.max_uri_handlers = 3;
	httpd_handle_t server = NULL;
	if (httpd_start(&server,&config) == ESP_OK){
		httpd_register_uri_handler(server,&root_get_uri);
		httpd_register_uri_handler(server,&root_post_uri);
		httpd_register_uri_handler(server,&fake_uri);
	}
	return server;
}



void start_up_wifi(void) {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ESP_ERROR_CHECK(nvs_flash_init());
	}
	ESP_LOGI(TAG,"ESP_WIFI_MODE_AP");
	wifi_init_softap();
	update_root_resp();
	start_httpd_server();
}
