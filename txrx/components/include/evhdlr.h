#ifndef EVHDLR_H
#define EVHDLR_H

#include <string.h>
#include "wifi.h"
#include "eth.h"
#include "mb.h"

#ifdef TAG
#undef TAG
#define TAG "evhdlr"
#endif



void set_r_nc(char *ip4_str, int state);
void start_evhdlr(void);

#endif
