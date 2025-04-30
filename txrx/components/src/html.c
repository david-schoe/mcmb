#include "html.h"
#include "nvs.h"

// buffer to hold the entire HTML response string
char root_html[ROOT_HTML_SZ];

// Rebuild root_html string dynamically from current values
esp_err_t update_root_html(void) {
    memset(root_html, 0, ROOT_HTML_SZ);
    int err = snprintf(root_html, ROOT_HTML_SZ,
        "<!DOCTYPE html><html><head><title>ESP32 Config</title></head><body>"
        "<h1>ESP32 Modbus Configuration</h1><form method=\"POST\" action=\"/\">"

        "<section><h2>Ethernet (eth)</h2>"
        "<label>IP4:</label><input name=\"eth_ip4_str\" value=\"%s\" /><br>"
        "<label>GATEWAY:</label><input name=\"eth_gw_str\" value=\"%s\" /><br>"
        "<label>NETMASK:</label><input name=\"eth_nm_str\" value=\"%s\" /></section>"

        "<section><h2>Wifi AP (ap)</h2>"
        "<label>SSID:</label><input name=\"ap_ssid_str\" value=\"%s\" /><br>"
        "<label>IP4:</label><input name=\"ap_ip4_str\" value=\"%s\" /><br>"
        "<label>GATEWAY:</label><input name=\"ap_gw_str\" value=\"%s\" /><br>"
        "<label>NETMASK:</label><input name=\"ap_nm_str\" value=\"%s\" /></section>"

        "<section><h2>Wifi STA (sta)</h2>"
        "<label>SSID:</label><input name=\"sta_ssid_str\" value=\"%s\" /><br>"
        "<label>IP4:</label><input name=\"sta_ip4_str\" value=\"%s\" /><br>"
        "<label>GATEWAY:</label><input name=\"sta_gw_str\" value=\"%s\" /><br>"
        "<label>NETMASK:</label><input name=\"sta_nm_str\" value=\"%s\" /></section>"

        "<section><h2>Read Registers</h2>"
        "<label>READ REG0:</label><input name=\"r_reg_str0\" value=\"%s\" />"
        "<label>low value</label><input name=\"r_reg_lo_str0\" value=\"%s\" />"
        "<label>high value</label><input name=\"r_reg_hi_str0\" value=\"%s\" /><br>"
        "<label>READ REG1:</label><input name=\"r_reg_str1\" value=\"%s\" />"
        "<label>low value</label><input name=\"r_reg_lo_str1\" value=\"%s\" />"
        "<label>high value</label><input name=\"r_reg_hi_str1\" value=\"%s\" /><br>"
        "<label>READ REG2:</label><input name=\"r_reg_str2\" value=\"%s\" />"
        "<label>low value</label><input name=\"r_reg_lo_str2\" value=\"%s\" />"
        "<label>high value</label><input name=\"r_reg_hi_str2\" value=\"%s\" /><br>"
        "<label>READ REG3:</label><input name=\"r_reg_str3\" value=\"%s\" />"
        "<label>low value</label><input name=\"r_reg_lo_str3\" value=\"%s\" />"
        "<label>high value</label><input name=\"r_reg_hi_str3\" value=\"%s\" /><br>"
        "<label>READ BIND IP4:</label><input name=\"rb_ip4_str\" value=\"%s\" /><br>"
        "<label>READ CONN IP4:</label><input name=\"rc_ip4_str\" value=\"%s\" /></section>"

        "<section><h2>Write Registers</h2>"
        "<label>WRITE REG0:</label><input name=\"w_reg_str0\" value=\"%s\" />"
        "<label>low value</label><input name=\"w_reg_lo_str0\" value=\"%s\" />"
        "<label>high value</label><input name=\"w_reg_hi_str0\" value=\"%s\" /><br>"
        "<label>WRITE REG1:</label><input name=\"w_reg_str1\" value=\"%s\" />"
        "<label>low value</label><input name=\"w_reg_lo_str1\" value=\"%s\" />"
        "<label>high value</label><input name=\"w_reg_hi_str1\" value=\"%s\" /><br>"
        "<label>WRITE REG2:</label><input name=\"w_reg_str2\" value=\"%s\" />"
        "<label>low value</label><input name=\"w_reg_lo_str2\" value=\"%s\" />"
        "<label>high value</label><input name=\"w_reg_hi_str2\" value=\"%s\" /><br>"
        "<label>WRITE REG3:</label><input name=\"w_reg_str3\" value=\"%s\" />"
        "<label>low value</label><input name=\"w_reg_lo_str3\" value=\"%s\" />"
        "<label>high value</label><input name=\"w_reg_hi_str3\" value=\"%s\" /><br>"
        "<label>WRITE BIND IP4:</label><input name=\"wb_ip4_str\" value=\"%s\" /><br>"
        "<label>WRITE CONN IP4:</label><input name=\"wc_ip4_str\" value=\"%s\" /></section>"

        "<input type=\"submit\" value=\"Save Configuration\"></form>"
        "<form method=\"GET\" action=\"/restart\">"
        "<input type=\"submit\" value=\"Restart\" style=\"background-color:red;color:white;\"></form>"
        "</body></html>",

        eth_ip4_str,eth_gw_str,eth_nm_str,
        ap_ssid_str,ap_ip4_str,ap_gw_str,ap_nm_str,
        sta_ssid_str,sta_ip4_str,sta_gw_str,sta_nm_str,
        r_reg_str[0],r_reg_lo_str[0],r_reg_hi_str[0],
        r_reg_str[1],r_reg_lo_str[1],r_reg_hi_str[1],
        r_reg_str[2],r_reg_lo_str[2],r_reg_hi_str[2],
        r_reg_str[3],r_reg_lo_str[3],r_reg_hi_str[3],
        rb_ip4_str,rc_ip4_str,
        w_reg_str[0],w_reg_lo_str[0],w_reg_hi_str[0],
        w_reg_str[1],w_reg_lo_str[1],w_reg_hi_str[1],
        w_reg_str[2],w_reg_lo_str[2],w_reg_hi_str[2],
        w_reg_str[3],w_reg_lo_str[3],w_reg_hi_str[3],
        wb_ip4_str,wc_ip4_str
    );
    return err;
}
