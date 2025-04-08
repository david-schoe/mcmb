// removed extern declaration due to redundancy

#ifndef HTTPD_H
#define HTTPD_H

#include "esp.h"
#include "html.h"

<<<<<<< HEAD
#define MAXB 64
#define MAXR 2048


#ifdef TAG
#undef TAG
#endif
#define TAG "httpd"

extern char control_0[MAXB];
extern char control_1[MAXB];
extern char control_2[MAXB];
extern char control_3[MAXB];

extern int err;

=======
>>>>>>> 5d0caaaf4d3097c82e7135141935257951de72ea
httpd_handle_t start_httpd(void);

#endif
