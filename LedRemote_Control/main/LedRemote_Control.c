//Chuong trinh de dieu khien Esp32 tu xa thong qua MQTT
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"

#define CREDENTIALS_PSSW     "huuphucQ2E4T6U8O0"
#define CREDENTIALS_USERNAME "LuongHuu_Phuc"
#define WIFI_SSID            "nubia Neo 5G"
#define WIFI_PASSWORD        "qqqqqqqq"
#define LED_PIN              GPIO_NUM_2

static esp_mqtt_client_handle_t mqtt_client = NULL;
static const char *TAG = "MQTT_LED_REMOTE";

extern const uint8_t isrgrootx1_pem_start[] asm("_binary_isrgrootx1_pem_start");
extern const uint8_t isrgrootx1_pem_end[] asm("_binary_isrgrootx1_pem_end");

void set_dns(void){
  ip_addr_t dnsserver;
  IP4_ADDR(&dnsserver.u_addr.ip4, 8, 8, 8, 8);
  dnsserver.type = IPADDR_TYPE_V4;
  dns_setserver(0, &dnsserver);
}

void wifi_init_sta(void){
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASSWORD
    },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "Wifi started, connecting...");
  ESP_ERROR_CHECK(esp_wifi_connect());
  set_dns();
}

// Ham xu ly du lieu MQTT
static void handle_mqtt_led_command(const char *data, int len){
  if(strncmp(data, "ON", len) == 0){
    gpio_set_level(LED_PIN, 1);
    ESP_LOGI(TAG, "LED ON !");
    esp_mqtt_client_publish(mqtt_client, "esp32/led/status", "ON", 0, 1, 0);
  }else if(strncmp(data, "OFF", len) == 0){
    gpio_set_level(LED_PIN, 0);
    ESP_LOGI(TAG, "LED OFF !");
    esp_mqtt_client_publish(mqtt_client, "esp32/led/status", "OFF", 0, 1, 0);
  }else{
    ESP_LOGW(TAG, "Unknown command: %.*s", len, data);
  }
}

// MQTT callback
static void mqtt_event_handle_callback(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
  esp_mqtt_event_handle_t event = event_data;
  mqtt_client = event->client;

  switch(event->event_id){
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT connected");
      esp_mqtt_client_subscribe(mqtt_client, "esp32/led/control", 1);
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGE(TAG, "MQTT disconnected");
      break;
    case MQTT_EVENT_DATA:
      ESP_LOGI(TAG, "Received on topic: %.*s", event->topic_len, event->topic);
      ESP_LOGI(TAG, "Data: %.*s", event->data_len, event->data);
      if(strncmp(event->topic, "esp32/led/control", event->topic_len) == 0){
        handle_mqtt_led_command(event->data, event->data_len); 
      }
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGE(TAG, "MQTT_EVENT_ERROR !");
      break;
    default:
      break;
  }
}

static esp_err_t mqtt_app_start(void){
  const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtts://f20005c3b8af405e8b1229f7da174402.s1.eu.hivemq.cloud:8883",
    .broker.verification.certificate = (const char*)isrgrootx1_pem_start,
    .credentials = {
      .authentication.password = CREDENTIALS_PSSW,
      .username = CREDENTIALS_USERNAME
    },
  };

  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handle_callback, NULL);
  return esp_mqtt_client_start(mqtt_client);
}

void led_config(void){
  gpio_config_t io_config = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pin_bit_mask = (1UL << LED_PIN)
  };
  gpio_config(&io_config);
  gpio_set_level(LED_PIN, 0);
}

void app_main(void){
  esp_err_t ret = nvs_flash_init();
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  }

  led_config();
  wifi_init_sta();
  vTaskDelay(pdMS_TO_TICKS(4000));
  ESP_ERROR_CHECK(mqtt_app_start());
}