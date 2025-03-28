#ifndef WIFI_H
#define WIFI_H

#include <string.h>
#include "esp.h"

#define WLAN0_PASS		""
#define WLAN0_CHAN		1
#define WLAN0_STA		4
#define WLAN0_HID       0
#define WLAN0_GATEWAY  "192.168.1.1"
#define WLAN0_NET_MASK  "255.255.255.0"

#define WLAN1_PASS      ""
#define WLAN1_CHAN      1
#define WLAN1_GATEWAY  "192.168.2.1"
#define WLAN1_NET_MASK  "255.255.255.0"

#ifdef TAG
#undef TAG
#endif
#define TAG "wifi"

// enable ip_forward in menuconfig
// change to force pbuf_add_header in esp-idf/components/lwip/lwip/src/core/pbuf.c


extern esp_netif_t *wlan0;
extern esp_netif_t *wlan1;
extern char wlan0_ip4_str[16];
extern char wlan1_ip4_str[16];
extern char wlan0_ssid_str[32];
extern char wlan1_ssid_str[32];

void init_wifi(void);
void connect_wifi(void);
void stop_wifi(void);
void start_wifi(void);

#endif
