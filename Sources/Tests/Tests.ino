/**
 * Минимальный тест джойстика для ESP32
 */
const int X_PIN = 32;
const int Y_PIN = 33;
const int BUTTON_PIN_1 = 25;
const int BUTTON_PIN_2 = 26;
const int BUTTON_PIN_3 = 35;
const int BUTTON_PIN_4 = 34;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC     16   // Data/Command
#define OLED_RST    17   // Reset
#define OLED_CS     19   // Chip Select

// Конструктор: width, height, &SPI, DC, RST, CS
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);
void setup() {
  Serial.begin(9600);
  delay(1000);
  // while(!Serial); // Ждём подключения монитора
  Serial.println("=== Start ===");
  
  SPI.begin(18, 12, 23, OLED_CS);
  Serial.println("SPI initialized");
  
  Serial.print("Trying display.begin()... ");
  bool result = display.begin(SSD1306_SWITCHCAPVCC, 0);
  Serial.println(result ? "OK" : "FAILED");
  
  if(!result) {
    Serial.println("💡 Tips:");
    Serial.println("  1. Check VCC = 3.3V");
    Serial.println("  2. Try SSD1306 library with I2C");
    Serial.println("  3. Your display might be SSD1309 (different init)");
    for(;;);
  }
  
  Serial.println("Drawing...");
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("TEST");
  display.display();
  Serial.println("Done!");

  // Serial.begin(9600);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  analogReadResolution(12);
  
  Serial.println("ESP32 Joystick Test");
  Serial.println("X\tY\tButton");
}
void loop() {
  int x = analogRead(X_PIN);
  int y = analogRead(Y_PIN);
  int btn_1 = digitalRead(BUTTON_PIN_1);
  int btn_2 = digitalRead(BUTTON_PIN_2);
  int btn_3 = digitalRead(BUTTON_PIN_3);
  int btn_4 = digitalRead(BUTTON_PIN_4);
  
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(btn_1 == LOW ? "1_PRESSED" : "---");
  Serial.print("\t");
  Serial.print(btn_2 == LOW ? "2_PRESSED" : "---");
  Serial.print("\t");
  Serial.print(btn_3 == HIGH ? "3_PRESSED" : "---");
  Serial.print("\t");
  Serial.println(btn_4 == HIGH ? "4_PRESSED" : "---");
  
  delay(50);
  // Можно добавить обновление текста в реальном времени
  // Например, мигающий курсор или счётчик
}
