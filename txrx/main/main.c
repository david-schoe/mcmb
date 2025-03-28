#include "netif.h"
#include "eth.h"
#include "wifi.h"
#include "httpd.h"
#include "html.h"
#include "nvs.h"
#include "mb.h"
#include "evhdlr.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "main"



void app_main(void){
    start_netif();
    start_nvs();
    start_evhdlr();
    start_mb();
    start_wifi();
    start_httpd();
    // start_eth();
    for (;;){
        vTaskDelay(100);
    }
}
