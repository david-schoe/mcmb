// updated validate_body() and validate_control() logic
// connects to new html_post_handler() system
// uses update_root_html() and HTML rendering in root_get_handler()

#include "httpd.h"
#include "html.h"

#define TAG "httpd"

int err;

// GET / handler
esp_err_t root_get_handler(httpd_req_t *req) {
    update_root_html();
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// POST / handler
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
        #define HANDLE(k, v) if (httpd_query_key_value(content, k, v, MAX_FIELD_LEN) == ESP_OK) { \
            nvs_set_str(nvs, k, v); }

        HANDLE("eth_ip4_str", eth_ip4_str); HANDLE("wlan0_ip4_str", wlan0_ip4_str); HANDLE("wlan1_ip4_str", wlan1_ip4_str);
        HANDLE("r_reg0_str", r_reg0_str); HANDLE("r_reg1_str", r_reg1_str);
        HANDLE("r_reg2_str", r_reg2_str); HANDLE("r_reg3_str", r_reg3_str);
        HANDLE("rc_ip4_str", rc_ip4_str); HANDLE("rb_ip4_str", rb_ip4_str);
        HANDLE("w_reg0_str", w_reg0_str); HANDLE("w_reg1_str", w_reg1_str);
        HANDLE("w_reg2_str", w_reg2_str); HANDLE("w_reg3_str", w_reg3_str);
        HANDLE("wc_ip4_str", wc_ip4_str); HANDLE("wb_ip4_str", wb_ip4_str);

        nvs_commit(nvs);
        nvs_close(nvs);
    }

    update_root_html();
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
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
	config.max_uri_handlers = 3;
	
	httpd_handle_t server = NULL;
	if (httpd_start(&server,&config) == ESP_OK){
		httpd_register_uri_handler(server,&root_get_uri);
		httpd_register_uri_handler(server,&root_post_uri);
		httpd_register_uri_handler(server,&fake_uri);
	}
	return server;
}
