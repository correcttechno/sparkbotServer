#include <WiFi.h>
#include <memory.h>
#include <serverFlash.h>
#include <PubSubClient.h>
//
const char *mqtt_server = "sparkbot.correcttechno.com";
const int mqtt_port = 1883;
const char *topic_chunk = "device/123/ota";
const char *topic_size = "device/123/ota_size";

WiFiClient espClient;
PubSubClient client(espClient);

// Firmware boyutu ve sayaç
size_t firmware_size = 0;
size_t received_bytes = 0;
static int chunk_counter = 0;

// OTA buffer
static uint8_t otaBuff[1024];

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

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    // Firmware boyutu mesajı
    if (strcmp(topic, topic_size) == 0)
    {
        char buf[20];
        memcpy(buf, payload, length);
        buf[length] = '\0';
        firmware_size = atoi(buf);
        received_bytes = 0;
        chunk_counter = 0;
        Serial.printf("Firmware boyutu alındı: %d byte\n", (int)firmware_size);
        return;
    }

    // Firmware chunk alımı
    if (!Update.isRunning())
    {
        if (!Update.begin(firmware_size))
        {
            Serial.printf("Update.begin hatası: %s\n", Update.errorString());
            return;
        }
    }

    size_t written = Update.write(payload, length);
    received_bytes += written;
    chunk_counter++;
    Serial.printf("Chunk #%d alındı: %d byte, toplam: %d/%d\n",
                  chunk_counter, (int)written, (int)received_bytes, (int)firmware_size);

    // Tüm firmware alındıysa
    if (received_bytes >= firmware_size)
    {
        if (Update.end(true))
        {
            Serial.println("OTA tamamlandı! Cihaz yeniden başlatılıyor...");
            ESP.restart();
        }
        else
        {
            Serial.printf("Update.end hatası: %s\n", Update.errorString());
        }
    }
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("MQTT broker'a bağlanılıyor...");
        if (client.connect("ESP32_OTA_Client"))
        {
            Serial.println("Bağlandı!");
            client.subscribe(topic_chunk);
            client.subscribe(topic_size);
        }
        else
        {
            Serial.print("Hata, rc=");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

void startMQTTClient()
{
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqtt_callback);

    for (;;) // Sonsuz döngü ama FreeRTOS uyumlu
    {
        if (!client.connected())
        {
            reconnect();
        }
        client.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS); // ✅ FreeRTOS’a nefes aldır
    }
}

struct TaskParams
{
    String str1;
    String str2;
};

void startCallBack(void *pvParameters)
{
    TaskParams *params = (TaskParams *)pvParameters;
    beginWifi(params->str1, params->str2);
    startMQTTClient();
}

String startSparkbot(String ssid, String password)
{

    static TaskParams params = {ssid, password};
    // WebSocket'i Core 0 üzerinde ayrı task olarak başlat
    xTaskCreatePinnedToCore(
        startCallBack, // Fonksiyon
        "startCallBack",
        8192,    // Stack size
        &params, // Parametre
        1,       // Öncelik
        NULL,    // Task handle
        0        // Core ID (0 veya 1)
    );
    return String(WiFi.macAddress());
}