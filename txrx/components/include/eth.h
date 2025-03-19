#ifndef ETH_H
#define ETH_H

#include "esp.h"

#define SPI_BUS SPI2_HOST
#define MISO_IO_NUM 12
#define MOSI_IO_NUM 26
#define SCLK_IO_NUM 14
#define SPICS_IO_NUM 15
#define INT_IO_NUM 16
#define RST_IO_NUM 17
#define CLOCK_SPEED_HZ 16000000

#define ESP_32_MAC_ADDR "daveys"
#define ESP_32_IP4_ADDR "192.168.1.1"
#define ESP_32_NET_MASK "255.255.0.0"
#define DAT_8024_IP4_ADDR "192.168.1.174"



esp_netif_t *start_eth(void);

#endif
