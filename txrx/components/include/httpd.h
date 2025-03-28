#ifndef HTTPD_H
#define HTTPD_H

#include "esp.h"
#include "html.h"

#define MAXB 64
#define MAXR 2048

// replace old variables
extern char output_ip1[MAXB];
extern char output_ip2[MAXB];
extern char output_ip3[MAXB];
extern char output_ip4[MAXB];

extern int err;

httpd_handle_t start_httpd(void);

#endif
