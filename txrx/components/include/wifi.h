#ifndef WIFI_H
#define WIFI_H

#include <string.h>
#include "esp.h"

#define AP_PASS		""
#define AP_CHAN		1
#define AP_STA		4
#define AP_HID       0

#define STA_PASS      ""
#define STA_CHAN      1

#ifdef TAG
#undef TAG
#endif
#define TAG "wifi"

// enable ip_forward in menuconfig
// change to force pbuf_add_header in esp-idf/components/lwip/lwip/src/core/pbuf.c:513


extern esp_netif_t *ap;
extern esp_netif_t *sta;
extern char ap_ip4_str[16];
extern char sta_ip4_str[16];
extern char ap_gw_str[16];
extern char sta_gw_str[16];
extern char ap_nm_str[16];
extern char sta_nm_str[16];
extern char ap_ssid_str[32];
extern char sta_ssid_str[32];

void init_wifi(void);
void connect_wifi(void);
void stop_wifi(void);
void start_wifi(void);

#endif
