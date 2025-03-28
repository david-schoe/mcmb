// declares functions and shared variables used by HTML UI rendering and HTTP form handling

#ifndef HTML_H
#define HTML_H

#include "esp.h"
#include "httpd.h"

#define ROOT_HTML_SZ 2048

// buffer that holds the dynamically generated HTML page
extern char root_html[ROOT_HTML_SZ];

// rebuilds root_html[] with updated values from RAM
esp_err_t update_root_html(void);

// handler for POST / — parses incoming form and updates fields + NVS
esp_err_t html_post_handler(httpd_req_t *req);

// handler for GET / — sends current root_html
esp_err_t html_get_handler(httpd_req_t *req);

// loads all saved textbox values from NVS into global variables
void load_all_from_nvs(void);

// converts a string-formatted IP into uint32_t (for sockets)
bool ip_str_to_u32(const char *ip_str, uint32_t *ip_u32);

// checks whether two IP addresses belong to the same subnet
bool is_same_subnet(uint32_t ip1, uint32_t ip2, uint32_t mask);

#endif
