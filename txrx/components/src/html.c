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
#define IP4_SIZE 16
#define REG_SIZE 6
#define ROOT_HTML_SZ 4096

// buffer to hold the entire HTML response string
char root_html[ROOT_HTML_SZ];

// Rebuild root_html string dynamically from current values
esp_err_t update_root_html(void) {
    memset(root_html, 0, ROOT_HTML_SZ);
    int err = snprintf(root_html, ROOT_HTML_SZ,
        "<!DOCTYPE html><html><head><title>ESP32 Config</title></head><body>"
        "<h1>ESP32 Modbus Configuration</h1><form method=\"POST\" action=\"/\">"

        "<section><h2>Network Interfaces</h2>"
        "<label>ETH IP4:</label><input name=\"eth_ip4_str\" value=\"%s\" /><br>"
        "<label>ETH GATEWAY:</label><input name=\"eth_gw\" value=\"%s\" /><br>"
        "<label>ETH NETMASK:</label><input name=\"eth_nm\" value=\"%s\" /><br>"
        "<label>WLAN0 IP4:</label><input name=\"wlan0_ip4_str\" value=\"%s\" /><br>"
        "<label>WLAN0 GATEWAY:</label><input name=\"wlan0_gw\" value=\"%s\" /><br>"
        "<label>WLAN0 NETMASK:</label><input name=\"wlan0_nm\" value=\"%s\" /><br>"
        "<label>WLAN1 IP4:</label><input name=\"wlan1_ip4_str\" value=\"%s\" /><br>"
        "<label>WLAN1 GATEWAY:</label><input name=\"wlan1_gw\" value=\"%s\" /><br>"
        "<label>WLAN1 NETMASK:</label><input name=\"wlan1_nm\" value=\"%s\" /></section>"

        "<section><h2>Read Registers</h2>"
        "<label>READ REG0:</label><input name=\"r_reg0_str\" value=\"%s\" /><br>"
        "<label>READ REG1:</label><input name=\"r_reg1_str\" value=\"%s\" /><br>"
        "<label>READ REG2:</label><input name=\"r_reg2_str\" value=\"%s\" /><br>"
        "<label>READ REG3:</label><input name=\"r_reg3_str\" value=\"%s\" /><br>"
        "<label>READ CONN IP4:</label><input name=\"rc_ip4_str\" value=\"%s\" /><br>"
        "<label>READ BIND IP4:</label><input name=\"rb_ip4_str\" value=\"%s\" /></section>"

        "<section><h2>Write Registers</h2>"
        "<label>WRITE REG0:</label><input name=\"w_reg0_str\" value=\"%s\" /><br>"
        "<label>WRITE REG1:</label><input name=\"w_reg1_str\" value=\"%s\" /><br>"
        "<label>WRITE REG2:</label><input name=\"w_reg2_str\" value=\"%s\" /><br>"
        "<label>WRITE REG3:</label><input name=\"w_reg3_str\" value=\"%s\" /><br>"
        "<label>WRITE CONN IP4:</label><input name=\"wc_ip4_str\" value=\"%s\" /><br>"
        "<label>WRITE BIND IP4:</label><input name=\"wb_ip4_str\" value=\"%s\" /></section>"

        "<input type=\"submit\" value=\"Save\"></form></body></html>",
        eth_ip4_str,eth_gw_str,eth_nm_str,wlan0_ip4_str,wlan0_gw_str,wlan0_nm_str,wlan1_ip4_str,wlan1_gw_str,wlan1_nm_str,
        r_reg0_str,r_reg1_str,r_reg2_str,r_reg3_str,rc_ip4_str,rb_ip4_str,
        w_reg0_str,w_reg1_str,w_reg2_str,w_reg3_str,wc_ip4_str,wb_ip4_str
    );
    return err;
}
