#ifndef ETH_H
#define ETH_H

#include "esp.h"

#define SPI_BUS SPI2_HOST
#define MISO_IO_NUM 12
#define MOSI_IO_NUM 13
#define SCLK_IO_NUM 14
#define SPICS_IO_NUM 15
#define INT_IO_NUM 16
#define RST_IO_NUM 17
#define CLOCK_SPEED_HZ 16000000

#define ETH_MAC_ADDR_ESP_32 "daveys"
#define ETH_IP_ADDR_ESP_32 "192.168.1.1"
#define ETH_NM_ESP_32 "255.255.0.0"
#define ETH_IP_ADDR_DAT_8024 "192.168.1.174"



esp_netif_t *start_eth(void);

#endif
