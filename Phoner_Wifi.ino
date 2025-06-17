#include <WiFi.h>
#include <HTTPClient.h>

#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "info.h"

/**
Current directory should contain "info.h" defining:
const char *ssid, the wifi ssid
const char *password, the wifi password
const char* botToken, the telegram bot token (@BotFather)
const char* chat_id, the telegram chat id (@userinfobot)
*/

constexpr gpio_num_t BTN_GPIO = GPIO_NUM_6 ;
constexpr int LED_PIN = GPIO_NUM_2;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };

  Serial.println("\nWiFi connected");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String message = "PHONER_EMERGENCY";
    String url = String("https://api.telegram.org/bot") + botToken +
                 "/sendMessage?chat_id=" + chat_id +
                 "&text=" + message;
    http.begin(url);
    int httpCode = http.GET();
    Serial.println("HTTP Status: " + String(httpCode));
    if (httpCode != 200) {
      String response = http.getString();
      Serial.println("Telegram Error: " + response);
    }
    http.end();
  }

  // Visual debug
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(10000);
  digitalWrite(LED_PIN, LOW);

  pinMode(BTN_GPIO, INPUT);
  /*
  rtc_gpio_pullup_dis(WAKEUP_PIN);
  rtc_gpio_pulldown_en(WAKEUP_PIN);
  rtc_gpio_init(WAKEUP_PIN);
  rtc_gpio_set_direction(WAKEUP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
  */

  esp_sleep_enable_ext1_wakeup((1ULL << BTN_GPIO), ESP_EXT1_WAKEUP_ANY_HIGH);
  // esp_sleep_enable_ext0_wakeup(BTN_GPIO, 1); // wake up if BTN pressed 
                                                // idt does not work ?? https://github.com/espressif/esp-idf/issues/11932
  rtc_gpio_pulldown_en(BTN_GPIO);  // GPIO6 is tie to GND in order to wake up in HIGH
  rtc_gpio_pullup_dis(BTN_GPIO);   // Disable PULL_UP in order to allow it to wakeup on HIGH
  // reminder : COM port is disabled, so board does show disconnected in IDE
  esp_deep_sleep_start();
}

void loop() {
  // nothing
}
