idf_component_register(SRCS "LedBlink.c"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES "isrgrootx1.pem"
                       PRIV_REQUIRES mqtt nvs_flash freertos driver esp_wifi)