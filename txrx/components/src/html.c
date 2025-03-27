// updated html.c file that includes:
// UI variable declarations
// NVS integration
// string to IP conversion
// subnet matching
// dynamic HTML generation
// POST handler that parses and stores user input


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/sockets.h"

#define TAG "html"
#define MAX_FIELD_LEN 64
#define ROOT_HTML_SZ 4096

char root_html[ROOT_HTML_SZ];

// UI Variables
char ip_eth[MAX_FIELD_LEN] = "";
char ip_wlan0[MAX_FIELD_LEN] = "";
char ip_wlan1[MAX_FIELD_LEN] = "";
char read_reg0[MAX_FIELD_LEN] = "";
char read_reg1[MAX_FIELD_LEN] = "";
char read_reg2[MAX_FIELD_LEN] = "";
char read_reg3[MAX_FIELD_LEN] = "";
char read_ip[MAX_FIELD_LEN] = "";
char read_netif_ip[MAX_FIELD_LEN] = "";
char write_reg0[MAX_FIELD_LEN] = "";
char write_reg1[MAX_FIELD_LEN] = "";
char write_reg2[MAX_FIELD_LEN] = "";
char write_reg3[MAX_FIELD_LEN] = "";
char write_ip[MAX_FIELD_LEN] = "";
char write_netif_ip[MAX_FIELD_LEN] = "";
char output_ip1[MAX_FIELD_LEN] = "";
char output_ip2[MAX_FIELD_LEN] = "";
char output_ip3[MAX_FIELD_LEN] = "";
char output_ip4[MAX_FIELD_LEN] = "";

// Convert string IP to uint32_t
bool ip_str_to_u32(const char *ip_str, uint32_t *ip_u32) {
    esp_ip4_addr_t ip4;
    if (esp_ip4addr_aton(ip_str, &ip4)) {
        *ip_u32 = ip4.addr;
        return true;
    }
    return false;
}

// Check if two IPs are in the same subnet
bool is_same_subnet(uint32_t ip1, uint32_t ip2, uint32_t mask) {
    return (ip1 & mask) == (ip2 & mask);
}

// Load a single field from NVS
void load_from_nvs(const char *key, char *dest) {
    nvs_handle_t nvs;
    size_t required_size;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs);
    if (err == ESP_OK) {
        err = nvs_get_str(nvs, key, NULL, &required_size);
        if (err == ESP_OK && required_size < MAX_FIELD_LEN) {
            nvs_get_str(nvs, key, dest, &required_size);
        }
        nvs_close(nvs);
    }
}

// Load all UI fields from NVS
void load_all_from_nvs() {
    load_from_nvs("ip_eth", ip_eth);
    load_from_nvs("ip_wlan0", ip_wlan0);
    load_from_nvs("ip_wlan1", ip_wlan1);
    load_from_nvs("read_reg0", read_reg0);
    load_from_nvs("read_reg1", read_reg1);
    load_from_nvs("read_reg2", read_reg2);
    load_from_nvs("read_reg3", read_reg3);
    load_from_nvs("read_ip", read_ip);
    load_from_nvs("read_netif_ip", read_netif_ip);
    load_from_nvs("write_reg0", write_reg0);
    load_from_nvs("write_reg1", write_reg1);
    load_from_nvs("write_reg2", write_reg2);
    load_from_nvs("write_reg3", write_reg3);
    load_from_nvs("write_ip", write_ip);
    load_from_nvs("write_netif_ip", write_netif_ip);
    load_from_nvs("output_ip1", output_ip1);
    load_from_nvs("output_ip2", output_ip2);
    load_from_nvs("output_ip3", output_ip3);
    load_from_nvs("output_ip4", output_ip4);
}

// Rebuild root_html string dynamically from current values
esp_err_t update_root_html(void) {
    memset(root_html, 0, ROOT_HTML_SZ);
    int err = snprintf(root_html, ROOT_HTML_SZ,
        "<!DOCTYPE html><html><head><title>ESP32 Config</title></head><body>"
        "<h1>ESP32 Modbus Configuration</h1><form method=\"POST\" action=\"/\">"

        "<section><h2>Network Interfaces</h2>"
        "<label>IP4 Ethernet:</label><input name=\"ip_eth\" value=\"%s\" /><br>"
        "<label>IP4 WLAN0:</label><input name=\"ip_wlan0\" value=\"%s\" /><br>"
        "<label>IP4 WLAN1:</label><input name=\"ip_wlan1\" value=\"%s\" /></section>"

        "<section><h2>Read Registers</h2>"
        "<label>REG0:</label><input name=\"read_reg0\" value=\"%s\" /><br>"
        "<label>REG1:</label><input name=\"read_reg1\" value=\"%s\" /><br>"
        "<label>REG2:</label><input name=\"read_reg2\" value=\"%s\" /><br>"
        "<label>REG3:</label><input name=\"read_reg3\" value=\"%s\" /><br>"
        "<label>Connect IP:</label><input name=\"read_ip\" value=\"%s\" /><br>"
        "<label>Bind NETIF IP:</label><input name=\"read_netif_ip\" value=\"%s\" /></section>"

        "<section><h2>Write Registers</h2>"
        "<label>REG0:</label><input name=\"write_reg0\" value=\"%s\" /><br>"
        "<label>REG1:</label><input name=\"write_reg1\" value=\"%s\" /><br>"
        "<label>REG2:</label><input name=\"write_reg2\" value=\"%s\" /><br>"
        "<label>REG3:</label><input name=\"write_reg3\" value=\"%s\" /><br>"
        "<label>Connect IP:</label><input name=\"write_ip\" value=\"%s\" /><br>"
        "<label>Bind NETIF IP:</label><input name=\"write_netif_ip\" value=\"%s\" /></section>"

        "<section><h2>Output Target IPs</h2>"
        "<label>Output IP 1:</label><input name=\"output_ip1\" value=\"%s\" /><br>"
        "<label>Output IP 2:</label><input name=\"output_ip2\" value=\"%s\" /><br>"
        "<label>Output IP 3:</label><input name=\"output_ip3\" value=\"%s\" /><br>"
        "<label>Output IP 4:</label><input name=\"output_ip4\" value=\"%s\" /></section>"

        "<input type=\"submit\" value=\"Save\"></form></body></html>",
        ip_eth, ip_wlan0, ip_wlan1,
        read_reg0, read_reg1, read_reg2, read_reg3, read_ip, read_netif_ip,
        write_reg0, write_reg1, write_reg2, write_reg3, write_ip, write_netif_ip,
        output_ip1, output_ip2, output_ip3, output_ip4
    );
    return err;
}

// GET / handler
esp_err_t html_get_handler(httpd_req_t *req) {
    update_root_html();
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// POST / handler
esp_err_t html_post_handler(httpd_req_t *req) {
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

        HANDLE("ip_eth", ip_eth); HANDLE("ip_wlan0", ip_wlan0); HANDLE("ip_wlan1", ip_wlan1);
        HANDLE("read_reg0", read_reg0); HANDLE("read_reg1", read_reg1);
        HANDLE("read_reg2", read_reg2); HANDLE("read_reg3", read_reg3);
        HANDLE("read_ip", read_ip); HANDLE("read_netif_ip", read_netif_ip);
        HANDLE("write_reg0", write_reg0); HANDLE("write_reg1", write_reg1);
        HANDLE("write_reg2", write_reg2); HANDLE("write_reg3", write_reg3);
        HANDLE("write_ip", write_ip); HANDLE("write_netif_ip", write_netif_ip);
        HANDLE("output_ip1", output_ip1); HANDLE("output_ip2", output_ip2);
        HANDLE("output_ip3", output_ip3); HANDLE("output_ip4", output_ip4);

        nvs_commit(nvs);
        nvs_close(nvs);
    }

    update_root_html();
    httpd_resp_send(req, root_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// URI registration
void register_html_uris(httpd_handle_t server) {
    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = html_get_handler,
        .user_ctx = NULL
    };

    httpd_uri_t uri_post = {
        .uri = "/",
        .method = HTTP_POST,
        .handler = html_post_handler,
        .user_ctx = NULL
    };

    httpd_register_uri_handler(server, &uri_get);
    httpd_register_uri_handler(server, &uri_post);
}
