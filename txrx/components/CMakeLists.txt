idf_component_register(SRCS "src/eth.c" "src/netif.c" "src/wlan.c" "src/nvs.c" "src/httpd.c" "src/html.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio
                        REQUIRES esp_event
                        REQUIRES esp_eth
                        REQUIRES esp_netif
                        REQUIRES esp_wifi
                        REQUIRES esp_http_server
                        REQUIRES nvs_flash)
