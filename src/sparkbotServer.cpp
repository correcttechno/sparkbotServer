#include <WiFi.h>
#include <memory.h>
#include <serverFlash.h>
#include <PubSubClient.h>

const char *mqtt_server = "broker.hivemq.com"; // test için public broker
const int mqtt_port = 1883;
const char *mqtt_topic_sub = "esp32/test/sub";
const char *mqtt_topic_pub = "esp32/test/pub";

WiFiClient espClient;
PubSubClient client(espClient);

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

// MQTT mesaj geldiğinde çalışacak callback fonksiyonu
void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Mesaj geldi [");
    Serial.print(topic);
    if(String(topic)=="update"){
        otaUpdate("otoupdate");
    }
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// MQTT bağlantısını tekrar kurmak için fonksiyon
void reconnect()
{
    // Bağlantı koparsa tekrar dene
    while (!client.connected())
    {
        Serial.print("MQTT baglantisi deneniyor...");
        // ClientID benzersiz olmalı
        String clientId = "ESP32Client-";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str()))
        {
            Serial.println("baglandi");
            // Abone ol
            client.subscribe(mqtt_topic_sub);
        }
        else
        {
            Serial.print("Baglanti hatasi, rc=");
            Serial.print(client.state());
            Serial.println(" 5 sn sonra tekrar denenecek");
            delay(5000);
        }
    }
}

void startCallBack(void *pvParameters)
{
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    for (;;) // Sonsuz döngü ama FreeRTOS uyumlu
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();

        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 5000)
        {
            lastMsg = millis();
            String msg = "Merhaba ESP32 MQTT!";
            client.publish(mqtt_topic_pub, msg.c_str());
            Serial.println("Mesaj gonderildi: " + msg);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // ✅ FreeRTOS’a nefes aldır
    }
}

String startSparkbot(String ssid, String password)
{
    beginWifi(ssid, password);
    // WebSocket'i Core 0 üzerinde ayrı task olarak başlat
    xTaskCreatePinnedToCore(
        startCallBack, // Fonksiyon
        "startCallBack",
        8192, // Stack size
        NULL, // Parametre
        1,    // Öncelik
        NULL, // Task handle
        0     // Core ID (0 veya 1)
    );
    return String(WiFi.macAddress());
}