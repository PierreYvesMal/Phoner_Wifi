#include <WiFi.h>
#include <HTTPClient.h>

#include "info.h"

/**
Current directory should contain "info.h" defining:
const char *ssid, the wifi ssid
const char *password, the wifi password
const char* botToken, the telegram bot token (@BotFather)
const char* chat_id, the telegram chat id (@userinfobot)
*/

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
}

void loop() {
  // nothing
}
