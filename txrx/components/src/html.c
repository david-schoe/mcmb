#include "html.h"

#define TAG "html"

char root_html[ROOT_HTML_SZ];

esp_err_t update_root_html(void) {

	memset(root_html,0,ROOT_HTML_SZ);
	err=sprintf(root_html,
		"<!DOCTYPE html><html><head><title>ESP32 Config</title></head><body>"
        "<h1>ESP32 Modbus Configuration</h1>"
        "<form method=\"POST\" action=\"/\">"

        // Network Interfaces
        "<section><h2>Network Interfaces</h2>"
        "<label>IP4 Ethernet:</label><input name=\"ip_eth\" value=\"%s\" /><br>"
        "<label>IP4 WLAN0:</label><input name=\"ip_wlan0\" value=\"%s\" /><br>"
        "<label>IP4 WLAN1:</label><input name=\"ip_wlan1\" value=\"%s\" /></section>"

        // Read Section
        "<section><h2>Read Registers</h2>"
        "<label>REG0:</label><input name=\"read_reg0\" value=\"%s\" /><br>"
        "<label>REG1:</label><input name=\"read_reg1\" value=\"%s\" /><br>"
        "<label>REG2:</label><input name=\"read_reg2\" value=\"%s\" /><br>"
        "<label>REG3:</label><input name=\"read_reg3\" value=\"%s\" /><br>"
        "<label>Connect IP:</label><input name=\"read_ip\" value=\"%s\" /><br>"
        "<label>Bind NETIF IP:</label><input name=\"read_netif_ip\" value=\"%s\" /></section>"

        // Write Section
        "<section><h2>Write Registers</h2>"
        "<label>REG0:</label><input name=\"write_reg0\" value=\"%s\" /><br>"
        "<label>REG1:</label><input name=\"write_reg1\" value=\"%s\" /><br>"
        "<label>REG2:</label><input name=\"write_reg2\" value=\"%s\" /><br>"
        "<label>REG3:</label><input name=\"write_reg3\" value=\"%s\" /><br>"
        "<label>Connect IP:</label><input name=\"write_ip\" value=\"%s\" /><br>"
        "<label>Bind NETIF IP:</label><input name=\"write_netif_ip\" value=\"%s\" /></section>"

        // Output IP Targets
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
