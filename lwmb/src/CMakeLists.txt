idf_component_register(SRCS "h.c" "r.c" "a.c"
                        INCLUDE_DIRS "../include"
                        REQUIRES esp_driver_gpio
                        REQUIRES esp_event
                        REQUIRES esp_eth
                        REQUIRES esp_netif)
