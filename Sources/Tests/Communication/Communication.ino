#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "BAMBUCHACHKIN";
const char* pass = "9|10r28A";
const IPAddress pcIP(192,168,137,1);  // IP ноутбука
const uint16_t pcPort = 8889;         // порт, который слушает ноутбук
const uint16_t espPort = 8888;        // порт ESP

WiFiUDP udp;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  udp.begin(espPort);
}

void loop() {
  // Отправка "пинга" каждые 5 секунд
  static uint32_t lastSend = 0;
  if (millis() - lastSend >= 5000) {
    lastSend = millis();
    udp.beginPacket(pcIP, pcPort);
    udp.print("ping");
    udp.endPacket();
    Serial.println("Sent ping");
  }
}