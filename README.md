# Sparkbot Kütüphanəsi (ESP32)

**Sparkbot** ESP32 üçün WebSocket (WSS) bağlantısı və OTA update funksionallığı təmin edən bir kütüphanədir. Bu kütüphane ilə cihazınızı **Sparkbot serverinə bağlaya**, mesaj göndərə və OTA update ala bilərsiniz.

---

## 📌 Xüsusiyyətləri
- WiFi bağlantısı
- WebSocket (WSS) dəstəyi
- OTA update funksiyası
- ESP32 üçün optimallaşdırılmış və paralel işləyə bilir

---

## ⚡ Quraşdırma (PlatformIO)
1. `lib/` qovluğuna Sparkbot kütüphanəsini əlavə edin:
lib/
└─ Sparkbot/
├─ Sparkbot.h
├─ Sparkbot.cpp
└─ library.json

markdown
Copy code
2. `platformio.ini` faylında ESP32 platformasını seçin.
3. `pio run` etdikdə `WebSockets` kütüphanəsi avtomatik yüklənəcək.

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
⚡ Quraşdırma (Arduino IDE)
Arduino IDE açın.

Library Manager vasitəsilə WebSockets (by Markus Sattler) kütüphanəsini yükləyin.

Sparkbot kütüphanəsini Arduino/libraries/ qovluğuna əlavə edin.

📌 İstifadəsi
cpp
Copy code
#include <Arduino.h>
#include "Sparkbot.h"

void setup() {
    Serial.begin(115200);

    // WiFi bağlantısı və Sparkbot serverinə qoşulma
    String mac = startSparkbot("SSID_ADI", "WIFI_SIFRESI");
    Serial.println("Cihaz MAC: " + mac);
}

void loop() {
    // Burada digər işləri edə bilərsiniz
    delay(2000);
}
startSparkbot(ssid, password) funksiyası cihazı WiFi şəbəkəsinə bağlayır və WebSocket taskini paralel olaraq başlatır.

Funksiya cihazın MAC ünvanını qaytarır.

📝 Qeyd
PlatformIO istifadəçiləri üçün library.json içində dependencies avtomatik yüklənir:

json
Copy code
"dependencies": {
  "WebSockets": {
    "url": "https://github.com/Links2004/arduinoWebSockets.git"
  }
}
Arduino IDE istifadəçiləri WebSockets kütüphanəsini manuel yükləməlidir.