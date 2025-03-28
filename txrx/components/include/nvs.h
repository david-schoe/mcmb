#ifndef NVS_H
#define NVS_H

#include "wifi.h"
#include "eth.h"
#include "mb.h"
#include "esp.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "nvs"

void start_nvs(void);

#endif
