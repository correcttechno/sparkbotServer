#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <memory.h>
#include <serverFlash.h>

const char *host = "sparkbot.correcttechno.com";
const int port = 321; // WSS default port

WebSocketsClient webSocket;

// WebSocket Event fonksiyonu
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_CONNECTED:
        Serial.println("✅ Connected to WSS server");
        webSocket.sendTXT("connectDevice|"+String(WiFi.macAddress()));
        break;

    case WStype_TEXT:
    {
        String message = String((char *)payload);
        Serial.println("📩 Mesaj geldi: " + message);
        if (message == "update")
            otaUpdate("otoupdate");
    }
    break;

    case WStype_DISCONNECTED:
        Serial.println("❌ Disconnected");
        break;

    default:
        break;
    }
}

// WebSocket için ayrı task
void WebSocketTask(void *pvParameters)
{
    // WSS bağlantısı
    webSocket.beginSSL(host, port, "/");
    webSocket.onEvent(webSocketEvent);

    while (true)
    {
        webSocket.loop();                    // WebSocket işlemleri sürekli dönmeli
        vTaskDelay(50 / portTICK_PERIOD_MS); // CPU %100 olmaması için küçük delay
    }
}

void beginWifi(String ssid, String password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n📡 WiFi connected");
}

String startSparkbot(String ssid, String password)
{
    beginWifi(ssid, password);
    // WebSocket'i Core 0 üzerinde ayrı task olarak başlat
    xTaskCreatePinnedToCore(
        WebSocketTask, // Fonksiyon
        "WebSocketTask",
        8192, // Stack size
        NULL, // Parametre
        1,    // Öncelik
        NULL, // Task handle
        0     // Core ID (0 veya 1)
    );
    return String(WiFi.macAddress());
}