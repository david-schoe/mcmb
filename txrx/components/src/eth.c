#include "eth.h"

#define TAG "eth"

esp_netif_t *start_eth(void){

    eth_mac_config_t eth_mac_cfg = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t eth_phy_cfg = ETH_PHY_DEFAULT_CONFIG();

    eth_phy_cfg.phy_addr = -1;
    eth_phy_cfg.reset_gpio_num = RST_IO_NUM;


    // install interrupt service routine for the spi-ethernet module (interrupt driven)
    gpio_install_isr_service(0);


    // initialize spi bus on SPI2_HOST
    spi_device_handle_t spi = NULL;
    spi_bus_config_t spi_bus_cfg = {
        .miso_io_num = MISO_IO_NUM,
        .mosi_io_num = MOSI_IO_NUM,
        .sclk_io_num = SCLK_IO_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_BUS, &spi_bus_cfg, 1));


    // set the spi_device_config to be used when creating eth_w5500_cfg
    spi_device_interface_config_t spi_dev_intrf_cfg = {
        .mode = 0,
        .clock_speed_hz = CLOCK_SPEED_HZ,
        .spics_io_num = SPICS_IO_NUM,
        .queue_size = 20
    };


    // create ethernet configuration specific to w5500
    eth_w5500_config_t eth_w5500_cfg = ETH_W5500_DEFAULT_CONFIG(SPI_BUS,&spi_dev_intrf_cfg);
    eth_w5500_cfg.int_gpio_num = INT_IO_NUM;

    esp_eth_mac_t *eth_mac = esp_eth_mac_new_w5500(&eth_w5500_cfg,&eth_mac_cfg);
    esp_eth_phy_t *eth_phy = esp_eth_phy_new_w5500(&eth_phy_cfg);

    esp_eth_config_t eth_cfg = ETH_DEFAULT_CONFIG(eth_mac,eth_phy);
    esp_eth_handle_t eth_hdl = NULL; // this is also the eth driver (esp_eth_driver_t)
    esp_eth_driver_install(&eth_cfg,&eth_hdl);

    // set mac address for eth_hdl (eth driver) which, more importantly, will set the mac of eth netif (set later with esp_eth_new_netif_glue)
    char *eth_mac_addr = ETH_MAC_ADDR_ESP_32;
    esp_eth_ioctl(eth_hdl,ETH_CMD_S_MAC_ADDR,(void*)eth_mac_addr);


    // create ethernet network interface
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth = esp_netif_new(&netif_cfg);
    esp_netif_attach(eth, esp_eth_new_netif_glue(eth_hdl));


    // in order to set a static ip address for the eth netif, Dynamic Host Configuration Protocol Client (dhcpc) must be stopped on the eth netif
    esp_netif_dhcpc_stop(eth);

    // set a static ip address for the eth netif
    esp_netif_ip_info_t eth_ip_info = { 0 };
    ip4addr_aton(ETH_IP_ADDR_ESP_32,(ip4_addr_t*)&eth_ip_info.ip);
    ip4addr_aton(ETH_NM_ESP_32,(ip4_addr_t*)&eth_ip_info.netmask);
    esp_netif_set_ip_info(eth,&eth_ip_info);

    // start ethernet
    esp_eth_start(eth_hdl);



    return eth;
}
