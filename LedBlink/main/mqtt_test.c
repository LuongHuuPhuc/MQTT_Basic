//Chuong trinh don gian bat tat led roi gui data den broker bang MQTT de hien thi len web HiveMQ
#include <stdio.h>
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
#include "lwip/dns.h"
#include "driver/gpio.h"
#include "lwip/ip_addr.h"

#define WIFI_SSID      "nubia Neo 5G"
#define WIFI_PASSWORD  "qqqqqqqq"
#define LED_PIN        GPIO_NUM_2

static const char *TAG = "MQTT_EXAMPLE"; //Luu vao bo nho BSS (Block Started by Symbol)
extern const uint8_t isrgrootx1_pem_start[] asm("_binary_isrgrootx1_pem_start");
extern const uint8_t isrgrootx1_pem_end[] asm("_binary_isrgrootx1_pem_end");
static esp_mqtt_client_handle_t mqtt_client = NULL;

void set_dns(void){
  ip_addr_t dnsserver;
  IP4_ADDR(&dnsserver.u_addr.ip4, 8, 8, 8, 8); //Set IP 8.8.8.8
  dnsserver.type = IPADDR_TYPE_V4; //Phai set type 
  dns_setserver(0, &dnsserver);
}

//===== WIFI Handler =====
void wifi_init_sta(void){
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_init_config);

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASSWORD
    },
  };
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "Wifi started, connecting...");
  ESP_ERROR_CHECK(esp_wifi_connect());
  set_dns();
}

//===== LED Blink Task =====
void led_blink_task(void *args){
  gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1UL << LED_PIN),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE
  };
  gpio_config(&io_conf);

  bool led_status = false;
  while(1){
    led_status = !led_status;
    gpio_set_level(LED_PIN, led_status);

    if(mqtt_client){
      const char *status = led_status ? "ON" : "OFF";
      esp_mqtt_client_publish(mqtt_client, "esp32/led/status", status, 0, 1, 0);
      ESP_LOGI(TAG, "Led is %s", status);
    }
    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

//==== MQTT Event Handler Callback ====
//Tu ban 4.4 tro di thi da chuan hoa toan bo he thong theo event loop
static void mqtt_event_handler_callback(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
  esp_mqtt_event_handle_t event = event_data;
  mqtt_client = event->client;

  switch(event->event_id){
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT connected");
      // esp_mqtt_client_publish(mqtt_client, "esp32/test", "Hello from Esp32", 0, 1, 0);
      xTaskCreate(led_blink_task, "led blink task", 2048, NULL, 5, NULL);
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGE(TAG, "MQTT disconnected");
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
      if(event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT){
        ESP_LOGE(TAG, "Last esp-tls error: 0x%x", event->error_handle->esp_tls_last_esp_err);
        ESP_LOGE(TAG, "TLS stack error: 0x%x", event->error_handle->esp_tls_stack_err);
        ESP_LOGE(TAG, "Socket errno: %d (%s)", event->error_handle->esp_transport_sock_errno, strerror(event->error_handle->esp_transport_sock_errno));
      }
      break;
    default:
      ESP_LOGI(TAG, "Other event id: %d", event->event_id);
      break;
  }
}

static esp_err_t mqtt_app_start(void){
  esp_err_t ret;
  const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtts://f20005c3b8af405e8b1229f7da174402.s1.eu.hivemq.cloud:8883",//Dia chi mqtt
    .broker.verification.certificate = (const char*)isrgrootx1_pem_start, //Root CA 
    .credentials = {
      .username = "LuongHuu_Phuc", //Username HiveMQ cloud
      .authentication.password = "huuphucQ2E4T6U8O0"
    },
  };

  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

  //Dang ky callback sau khi init
  ret = esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler_callback, NULL);
  if(ret != ESP_OK){
    ESP_LOGE(TAG, "MQTT client register FAILED !");
    return ret;
  }

  ret = esp_mqtt_client_start(mqtt_client);
  if(ret != ESP_OK){
    ESP_LOGE(TAG, "MQTT client start FAILED !");
    return ret;
  }
  ESP_LOGI(TAG, "MQTT client start OK !");
  return ret;
}

void app_main(void){
  esp_err_t ret = nvs_flash_init();

  if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
    nvs_flash_erase();
    nvs_flash_init();
  }

  wifi_init_sta();
  vTaskDelay(pdMS_TO_TICKS(4000)); //Cho Wifi on dinh
  ESP_ERROR_CHECK(mqtt_app_start());
}
