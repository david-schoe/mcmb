#include "evhdlr.h"


void set_r_nc(char *ip4_str, int state) {

    // ensure no process is modifying the remote tables
    xSemaphoreTake(h->re,portMAX_DELAY);
    struct rt *c = h->rt;
    int a = 0;
    ESP_LOGE(TAG,"ip4_str=%s, state=%d",ip4_str,state);
    while (1) {
        if (c == NULL) {
            xSemaphoreGive(h->re);
            break;
        }
        a++;
        if (!strcmp(c->r->b_ip4_str,ip4_str)) {
            c->r->nc = state;
             ESP_LOGI(TAG,"ip4_str=%s, state=%d",ip4_str,state);
        }
        c = c->n;
    }
}


static void evhdlr(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_START) {
            set_r_nc(wlan0_ip4_str,1);
        } else if (event_id == WIFI_EVENT_AP_STOP) {
            set_r_nc(wlan0_ip4_str,0);
        } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
            init_wifi();
            set_r_nc(wlan1_ip4_str,1);
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            set_r_nc(wlan1_ip4_str,0);
            connect_wifi();
        }
    }
    if (event_base == ETH_EVENT) {
        if (event_id == ETHERNET_EVENT_CONNECTED) {
            set_r_nc(eth_ip4_str,1);
            connect_eth();
        } else if (event_id == ETHERNET_EVENT_DISCONNECTED) {
            set_r_nc(eth_ip4_str,0);
            connect_eth();
        }
    }
}


void start_evhdlr(void) {
    esp_event_handler_instance_register(ESP_EVENT_ANY_BASE,ESP_EVENT_ANY_ID,evhdlr,NULL,NULL);
}
