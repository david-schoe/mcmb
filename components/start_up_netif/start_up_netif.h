#ifndef START_UP_NETIF_H
#define START_UP_NETIF_H

#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_netif.h"

void start_up_netif(void);

#endif
