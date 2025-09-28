#include "serverFlash.h"

// Global değişken tanımı
HTTPClient http;

bool otaUpdate(String filename)
{
    String url = "http://sparkbot.correcttechno.com/compiler/arduino/codes/" + filename + "/build/esp32.esp32.esp32/" + filename + ".ino.bin";
    Serial.println(url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        int contentLength = http.getSize();
        WiFiClient *stream = http.getStreamPtr();

        if (!Update.begin(contentLength)) {
            return false;
        }

        size_t written = Update.writeStream(*stream);
        if (written != contentLength) {
            Serial.printf("Eksik yazıldı: %d / %d\n", written, contentLength);
            return false;
        }

        if (Update.end()) {
            if (Update.isFinished()) {
                Serial.println("Başarılı! Yeniden başlatılıyor...");
                ESP.restart();
                return true;
            } else {
                Serial.println("Update bitmedi!");
                return false;
            }
        } else {
            Serial.printf("Update hatası: %s\n", Update.errorString());
            return false;
        }
    }
    else
    {
        Serial.printf("HTTP hata kodu: %d\n", httpCode);
        return false;
    }

    http.end();
}
