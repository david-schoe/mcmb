#include "netif.h"

#define TAG "netif"

void start_netif(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}
