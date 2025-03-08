#ifndef START_UP_WIFI_H
#define START_UP_WIFI_H

#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

#define ESP_WIFI_SSID		CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS		CONFIG_ESP_WIFI_PASS
#define ESP_WIFI_CHAN		CONFIG_ESP_WIFI_CHAN
#define ESP_MAX_STA	    	CONFIG_ESP_MAX_STA

void start_up_wifi(void);

#endif
