set(priv_req freertos mqtt driver esp_wifi nvs_flash)
idf_component_register(SRCS "LedRemote_Control.c"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES "isrgrootx1.pem"
                       PRIV_REQUIRES ${priv_req})