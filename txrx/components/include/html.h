#ifndef HTML_H
#define HTML_H

#include "esp.h"
#include "httpd.h"

#define ROOT_HTML_SZ 2048

extern char root_html[ROOT_HTML_SZ];
esp_err_t update_root_html(void);

#endif
