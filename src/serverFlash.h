#ifndef SERVER_FLASH_H
#define SERVER_FLASH_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

// Global değişken bildirimi
extern HTTPClient http;

// Fonksiyon prototipi
bool otaUpdate(String filename);

#endif
