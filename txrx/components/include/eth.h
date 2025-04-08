#ifndef ETH_H
#define ETH_H

#include "esp.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "eth"

#define SPI_BUS SPI2_HOST
#define MISO_IO_NUM 12
#define MOSI_IO_NUM 26
#define SCLK_IO_NUM 14
#define SPICS_IO_NUM 15
#define INT_IO_NUM 16
#define RST_IO_NUM 17
#define CLOCK_SPEED_HZ 16000000

#define ETH_MAC_ADDR "daveys"
#define DAT_8024_IP4_STR "192.168.1.174"


extern esp_netif_t *eth;
extern char eth_ip4_str[16];
extern char eth_gw_str[16];
extern char eth_nm_str[16];

void connect_eth(void);
esp_netif_t *start_eth(void);

#endif
