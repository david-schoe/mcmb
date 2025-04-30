// updated validate_body() and validate_control() logic
// connects to new html_post_handler() system
// uses update_root_html() and HTML rendering in root_get_handler()

#include "httpd.h"
#include "html.h"
#include "nvs.h"

int err;

esp_err_t root_get_handler(httpd_req_t *req) {
    update_root_html();
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t root_post_handler(httpd_req_t *req) {
    char content[1024];
    int total_len = req->content_len;
    int received = 0;
    int ret;

    while (received < total_len) {
        if ((ret = httpd_req_recv(req, content + received, total_len - received)) <= 0) {
            return ESP_FAIL;
        }
        received += ret;
    }
    content[received] = '\0';
    nvs_handle_t nvs;
    if (nvs_open("storage", NVS_READWRITE, &nvs) == ESP_OK) {
        #define HANDLE(k, v) switch (httpd_query_key_value(content, k, v, 17)) { \
			case ESP_OK: \
				nvs_set_str(nvs, k, v); \
				break; \
			case ESP_ERR_INVALID_ARG: \
				printf("ESP_ERR_INVALID_ARG\n"); \
				break; \
			case ESP_ERR_HTTPD_RESULT_TRUNC: \
				printf("ESP_ERR_HTTPD_RESULT_TRUNC\n"); \
				break; \
			case ESP_ERR_NOT_FOUND: \
				printf ("ESP_ERR_NOT_FOUND\n"); \
				break; \
			default: \
				break; \
		}

        HANDLE("eth_ip4_str", eth_ip4_str); HANDLE("ap_ip4_str", ap_ip4_str); HANDLE("sta_ip4_str", sta_ip4_str);
		HANDLE("eth_gw_str", eth_gw_str); HANDLE("ap_gw_str", ap_gw_str); HANDLE("sta_gw_str", sta_gw_str);
		HANDLE("eth_nm_str", eth_nm_str); HANDLE("ap_nm_str", ap_nm_str); HANDLE("sta_nm_str", sta_nm_str);
		HANDLE("ap_ssid_str",ap_ssid_str); HANDLE("sta_ssid_str",sta_ssid_str);
        HANDLE("r_reg_str0", r_reg_str[0]); HANDLE("r_reg_str1", r_reg_str[1]);
        HANDLE("r_reg_str2", r_reg_str[2]); HANDLE("r_reg_str3", r_reg_str[3]);
		HANDLE("r_reg_lo_str0", r_reg_lo_str[0]); HANDLE("r_reg_lo_str1", r_reg_lo_str[1]);
        HANDLE("r_reg_lo_str2", r_reg_lo_str[2]); HANDLE("r_reg_lo_str3", r_reg_lo_str[3]);
		HANDLE("r_reg_hi_str0", r_reg_hi_str[0]); HANDLE("r_reg_hi_str1", r_reg_hi_str[1]);
        HANDLE("r_reg_hi_str2", r_reg_hi_str[2]); HANDLE("r_reg_hi_str3", r_reg_hi_str[3]);
        HANDLE("rc_ip4_str", rc_ip4_str); HANDLE("rb_ip4_str", rb_ip4_str);
        HANDLE("w_reg_str0", w_reg_str[0]); HANDLE("w_reg_str1", w_reg_str[1]);
        HANDLE("w_reg_str2", w_reg_str[2]); HANDLE("w_reg_str3", w_reg_str[3]);
		HANDLE("w_reg_lo_str0", w_reg_lo_str[0]); HANDLE("w_reg_lo_str1", w_reg_lo_str[1]);
        HANDLE("w_reg_lo_str2", w_reg_lo_str[2]); HANDLE("w_reg_lo_str3", w_reg_lo_str[3]);
		HANDLE("w_reg_hi_str0", w_reg_hi_str[0]); HANDLE("w_reg_hi_str1", w_reg_hi_str[1]);
        HANDLE("w_reg_hi_str2", w_reg_hi_str[2]); HANDLE("w_reg_hi_str3", w_reg_hi_str[3]);
        HANDLE("wc_ip4_str", wc_ip4_str); HANDLE("wb_ip4_str", wb_ip4_str);

        nvs_commit(nvs);
        nvs_close(nvs);
    }

    update_root_html();
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t restart_get_handler(httpd_req_t *req) {
	char restart_get_html[512] =
		"<html>"
			"<body>"
				"<h2>Are you sure you want to restart esp32?</h2>"
				"<form method=\"POST\" action=\"/restart\"><input type=\"submit\" value=\"Yes (restart)\" style=\"color:white;background-color:red;\"></form>"
				"<form method=\"GET\" action=\"/\"><input type=\"submit\" value=\"No (cancel)\" /></form>"
			"</body>"
		"</html>";
    httpd_resp_send(req,restart_get_html,HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t restart_post_handler(httpd_req_t *req) {
	char restart_post_html[256] =
		"<html>"
			"<head>"
				"<meta http-equiv=\"refresh\" content=\"1; url=/\" />"
			"</head>"
			"<body>"
				"<h2>Restarting...</h2>"
			"</body>"
		"</html>";
	httpd_resp_send(req,restart_post_html,HTTPD_RESP_USE_STRLEN);
	vTaskDelay(10);
	esp_restart();
	return ESP_OK;
}

esp_err_t fake_handler(httpd_req_t *req) {
	return ESP_OK;
}


httpd_uri_t root_get_uri = {
	.uri = "/",
	.method = HTTP_GET,
	.handler = root_get_handler,
	.user_ctx = NULL
};


httpd_uri_t root_post_uri = {
	.uri = "/",
	.method = HTTP_POST,
	.handler = root_post_handler,
	.user_ctx = NULL
};


// confirmation screen
httpd_uri_t restart_get_uri = {
    .uri = "/restart",
    .method = HTTP_GET,
    .handler = restart_get_handler,
    .user_ctx = NULL
};

// performs the actual reset
httpd_uri_t restart_post_uri = {
    .uri = "/restart",
    .method = HTTP_POST,
    .handler = restart_post_handler,
    .user_ctx = NULL
};

// fake uri to supress 404
httpd_uri_t fake_uri = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = fake_handler,
    .user_ctx = NULL
};


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


httpd_handle_t start_httpd(void) {

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	update_root_html();
	config.max_uri_handlers = 5;
	
	httpd_handle_t server = NULL;
	if (httpd_start(&server,&config) == ESP_OK){
		httpd_register_uri_handler(server,&root_get_uri);
		httpd_register_uri_handler(server,&root_post_uri);
		httpd_register_uri_handler(server,&restart_get_uri);
		httpd_register_uri_handler(server,&restart_post_uri);
		httpd_register_uri_handler(server,&fake_uri);
	}
	return server;
}
