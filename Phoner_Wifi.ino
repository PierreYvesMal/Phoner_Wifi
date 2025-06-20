#include <WiFi.h>
#include <HTTPClient.h>

#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "info.h"

/**
Current directory should contain "info.h" defining:
struct WifiIds
{
  const char* ssid;
  const char* password;
};
constexpr WifiIds wifiIds[] =
{
  {"SSID1", "PWD1"}, // Maison parents
  {"SSID2", "PWD2"}, // Maison Loncin
};

const char* botToken, the telegram bot token (@BotFather)
const char* chat_id, the telegram chat id (@userinfobot)
*/

constexpr gpio_num_t BTN_GPIO = GPIO_NUM_6 ;
constexpr int LED_PIN = GPIO_NUM_2;

constexpr int MAX_TENTATIVES = 5;

bool ConnectToWifi();

void setup() {
  Serial.begin(115200);

  if (ConnectToWifi())
  {
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

bool ConnectToWifi()
{
  for (int i = 0; i < N_WIFI; ++i) 
  {
    int tentative = 0;
    const char* ssid = wifiIds[i].ssid;
    const char* password = wifiIds[i].password;

    Serial.println("Attempting to connect to wifi:");
    Serial.print("\t"); Serial.println(ssid);
    Serial.print("\t"); Serial.println(password);

    WiFi.begin(ssid, password);

    while ( (WiFi.status() != WL_CONNECTED) && (tentative++ < MAX_TENTATIVES) )
    {
      delay(500);
      Serial.print(".");
    };
    Serial.println("");

    if(WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nWiFi connected");
      return true;
    }

    WiFi.disconnect(true);  // true = erase old config
    delay(100); // give some time to cleanly disconnect
  }
  return false;
}
