// declares functions and shared variables used by HTML UI rendering and HTTP form handling

#ifndef HTML_H
#define HTML_H

#include "esp.h"
#include "httpd.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "html"


#define ROOT_HTML_SZ 4096
#define IP4_SIZE 16
#define REG_SIZE 6

// buffer that holds the dynamically generated HTML page
extern char root_html[4096];

// rebuilds root_html[] with updated values from RAM
esp_err_t update_root_html(void);

#endif
