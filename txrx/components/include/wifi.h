#ifndef WIFI_H
#define WIFI_H

#include "esp.h"

#define WLAN0_SSID      "192.168.4.1"
#define WLAN0_PASS      ""
#define WLAN0_CHAN      1


#define WLAN1_SSID		"192.168.4.1"
#define WLAN1_PASS		""
#define WLAN1_CHAN		1
#define WLAN1_STA		4
#define WLAN1_HID       0

#define WLAN1_IP4_ADDR  "192.168.4.1"
#define WLAN1_GATEWAY  "192.168.4.1"
#define WLAN1_NET_MASK  "255.255.255.0"


extern esp_netif_t *wlan0;
extern esp_netif_t *wlan1;

void start_wifi(void);

#endif
