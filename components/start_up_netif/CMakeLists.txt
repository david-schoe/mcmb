idf_component_register(SRCS "start_up_netif.c"
                        INCLUDE_DIRS "."
                        REQUIRES esp_event
                        REQUIRES esp_netif)
