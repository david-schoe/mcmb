#ifndef MB_H
#define MB_H
#include "h.h"


#ifdef TAG
#undef TAG
#endif
#define TAG "mb"

extern char rb_ip4_str[16];
extern char rc_ip4_str[16];
extern char wb_ip4_str[16];
extern char wc_ip4_str[16];

extern char r_reg_str[4][6];
extern char r_reg_lo_str[4][7];
extern char r_reg_hi_str[4][7];

extern char w_reg_str[4][6];
extern char w_reg_lo_str[4][7];
extern char w_reg_hi_str[4][7];


void start_mb(void);
#endif
