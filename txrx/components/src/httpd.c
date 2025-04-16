// updated httpd.c file that:
// - connects to new html_post_handler() system
// - uses update_root_html() and html_get_handler()
// - removes legacy validate_body/validate_control
// - registers reset confirmation and reset actions

#include "httpd.h"
#include "html.h"

#define TAG "httpd"

int err;

// External handlers from html.c
extern esp_err_t reset_confirm_handler(httpd_req_t *req);
extern esp_err_t reset_all_handler(httpd_req_t *req);

// GET / → returns the current HTML configuration page
esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(TAG, "root_get_handler: sent response");
    return ESP_OK;
}

// POST / → processes form submission via html_post_handler()
esp_err_t root_post_handler(httpd_req_t *req) {
    return html_post_handler(req);
}

// GET /favicon.ico → dummy handler to suppress 404
esp_err_t fake_handler(httpd_req_t *req) {
    return ESP_OK;
}

// GET /reset_confirm → confirmation screen
httpd_uri_t reset_confirm_uri = {
    .uri = "/reset_confirm",
    .method = HTTP_GET,
    .handler = reset_confirm_handler,
    .user_ctx = NULL
};

// POST /reset → performs the actual NVS + memory wipe
httpd_uri_t reset_handler = {
    .uri = "/reset",
    .method = HTTP_POST,
    .handler = reset_all_handler,
    .user_ctx = NULL
};

// Root GET/POST URIs
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

// Start the HTTP server and register all routes
httpd_handle_t start_httpd(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    update_root_html();  // build initial HTML

    config.max_uri_handlers = 5;  // includes reset + fake

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root_get_uri);
        httpd_register_uri_handler(server, &root_post_uri);
        httpd_register_uri_handler(server, &fake_uri);
        httpd_register_uri_handler(server, &reset_confirm_uri);
        httpd_register_uri_handler(server, &reset_handler);
    }
    return server;
}
