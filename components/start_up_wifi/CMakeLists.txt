idf_component_register(SRCS "start_up_wifi.c"
                        INCLUDE_DIRS "."
                        REQUIRES esp_wifi
                        REQUIRES esp_http_server
                        REQUIRES nvs_flash)
