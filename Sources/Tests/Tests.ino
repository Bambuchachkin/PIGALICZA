/**
 * Минимальный тест джойстика для ESP32
 */
const int X_PIN = 32;
const int Y_PIN = 33;
const int BTN_PIN = 25;

void setup() {
  Serial.begin(9600);
  pinMode(BTN_PIN, INPUT_PULLUP);
  analogReadResolution(12);
  
  Serial.println("ESP32 Joystick Test");
  Serial.println("X\tY\tButton");
}

void loop() {
  int x = analogRead(X_PIN);
  int y = analogRead(Y_PIN);
  int btn = digitalRead(BTN_PIN);
  
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.println(btn == LOW ? "PRESSED" : "---");
  
  delay(50);
}