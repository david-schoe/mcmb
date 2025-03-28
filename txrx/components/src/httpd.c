// updated validate_body() and validate_control() logic
// connects to new html_post_handler() system
// uses update_root_html() and HTML rendering in root_get_handler()

#include "httpd.h"
#include "html.h"

#define TAG "httpd"

int err;



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
	httpd_resp_send(req,root_html,HTTPD_RESP_USE_STRLEN);
	ESP_LOGI(TAG,"root_get_handler: sent response");
	return ESP_OK;
}


esp_err_t root_post_handler(httpd_req_t *req){
	return html_post_handler(req);  // redirect to html.c logic
}

// dummy favicon handler
esp_err_t fake_handler(httpd_req_t *req){
	return ESP_OK;
}


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




httpd_handle_t start_httpd(void) {

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	update_root_html();
	config.max_uri_handlers = 3;
	
	httpd_handle_t server = NULL;
	if (httpd_start(&server,&config) == ESP_OK){
		httpd_register_uri_handler(server,&root_get_uri);
		httpd_register_uri_handler(server,&root_post_uri);
		httpd_register_uri_handler(server,&fake_uri);
	}
	return server;
}
