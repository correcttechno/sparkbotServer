#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>




WiFiClient espClient;
HTTPClient http;

bool openWifi(String ssid, String password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    return true;
}

bool otaUpdate(String filename)
{
    String url=String("https://sparkbot.correcttechno.com/compiler/arduino/codes/"+filename+"/build/esp32.esp32.esp32/") + filename+".ino.bin";
    Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        int contentLength = http.getSize();
        WiFiClient *stream = http.getStreamPtr();

        if (!Update.begin(contentLength))
        {
            return false;
        }

        size_t written = Update.writeStream(*stream);
        if (written == contentLength)
        {
            Serial.println("Update tamamlandı!");
            return false;
        }
        else
        {
            Serial.printf("Eksik yazıldı: %d / %d\n", written, contentLength);
            return false;
        }

        if (Update.end())
        {
            if (Update.isFinished())
            {
                Serial.println("Başarılı! Yeniden başlatılıyor...");
                return true;
                ESP.restart();
            }
            else
            {
                Serial.println("Update bitmedi!");
                return false;
            }
        }
        else
        {
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
