#ifndef NETIF_H
#define NETIF_H

#include "esp.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "netif"

void start_netif(void);

#endif
