// removed extern declaration due to redundancy

#ifndef HTTPD_H
#define HTTPD_H

#include "esp.h"
#include "html.h"

#define MAXB 64
#define MAXR 2048


#ifdef TAG
#undef TAG
#endif
#define TAG "httpd"

httpd_handle_t start_httpd(void);

#endif
