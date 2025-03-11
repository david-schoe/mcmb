idf_component_register(SRCS "src/h.c" "src/r.c" "src/a.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio
                        REQUIRES esp_event
                        REQUIRES esp_eth
                        REQUIRES esp_netif)
