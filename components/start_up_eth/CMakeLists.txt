idf_component_register(SRCS "start_up_eth.c"
                        INCLUDE_DIRS "."
                        REQUIRES esp_driver_gpio
                        REQUIRES esp_event
                        REQUIRES esp_eth
                        REQUIRES esp_netif)
