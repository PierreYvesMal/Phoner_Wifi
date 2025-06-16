#include <WiFi.h>
#include <HTTPClient.h>

#include "esp_sleep.h"
#include "info.h"

/**
Current directory should contain "info.h" defining:
const char *ssid, the wifi ssid
const char *password, the wifi password
const char* botToken, the telegram bot token (@BotFather)
const char* chat_id, the telegram chat id (@userinfobot)
*/

constexpr gpio_num_t WAKEUP_PIN = GPIO_NUM_33;
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

  pinMode(WAKEUP_PIN, INPUT);
  /*
  rtc_gpio_pullup_dis(WAKEUP_PIN);
  rtc_gpio_pulldown_en(WAKEUP_PIN);
  rtc_gpio_init(WAKEUP_PIN);
  rtc_gpio_set_direction(WAKEUP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
  */

  Serial.println("Going to hibernation. Wake up on HIGH signal on GPIO 33.");
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1);
  esp_deep_sleep_start();
}

void loop() {
  // nothing
}
