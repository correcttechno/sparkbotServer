#ifndef SPARKBOT_H
#define SPARKBOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

// 📌 Dışarıya açılacak fonksiyonlar
void beginWifi(String ssid, String password);
String startSparkbot(String ssid, String password);

#endif // SPARKBOT_H
