#include "netif.h"
#include "eth.h"
#include "wifi.h"
#include "httpd.h"
#include "html.h"
#include "nvs.h"
#include "mb.h"

#define TAG "mcmb"

void app_main(void){
    start_netif();
    start_nvs();
    start_wifi();
    start_httpd();
    // start_eth();
    start_mb();
    for (;;){}
}
