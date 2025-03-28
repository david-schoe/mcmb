#ifndef MB_H
#define MB_H
#include "h.h"

#define R_REG(N) \
    htons((r_reg##N##_str[1]-0x30)*1000+(r_reg##N##_str[2]-0x30)*100+(r_reg##N##_str[3]-0x30)*10+(r_reg##N##_str[4]-0x30))
#define W_REG(N) \
    htons((w_reg##N##_str[1]-0x30)*1000+(w_reg##N##_str[2]-0x30)*100+(w_reg##N##_str[3]-0x30)*10+(w_reg##N##_str[4]-0x30))



#ifdef TAG
#undef TAG
#endif
#define TAG "mb"

extern char rb_ip4_str[16];
extern char rc_ip4_str[16];
extern char wb_ip4_str[16];
extern char wc_ip4_str[16];

extern char r_reg0_str[6];
extern char r_reg1_str[6];
extern char r_reg2_str[6];
extern char r_reg3_str[6];

extern char w_reg0_str[6];
extern char w_reg1_str[6];
extern char w_reg2_str[6];
extern char w_reg3_str[6];


void start_mb(void);
#endif
