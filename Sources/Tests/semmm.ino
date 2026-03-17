const int X_PIN = 32;
const int Y_PIN = 33;
const int BUTTON_PIN_1 = 25;
const int BUTTON_PIN_2 = 26;
const int BUTTON_PIN_3 = 35;
const int BUTTON_PIN_4 = 34;
SemaphoreHandle_t mutex; 
// pinMode(BUTTON_PIN_1, INPUT_PULLUP);
// pinMode(BUTTON_PIN_2, INPUT_PULLUP);
// pinMode(BUTTON_PIN_3, INPUT_PULLUP);
// pinMode(BUTTON_PIN_4, INPUT_PULLUP);
// analogReadResolution(12);
#define ARRSIZE  6
int buffer[ARRSIZE];
int arrr1[ARRSIZE];
int arrr2[ARRSIZE];
// SemaphoreHandle_t mutex;
// mutex = xSemaphoreCreateMutex();
// memset(&shared, 0, sizeof(SharedData));
void input_task(void *pvParameters){
  int val, value;
  while(1) { 
    buffer[0] = digitalRead(BUTTON_PIN_1);
    buffer[1] = digitalRead(BUTTON_PIN_2);
    buffer[2] = digitalRead(BUTTON_PIN_3);
    buffer[3] = digitalRead(BUTTON_PIN_4);
    // buffer[0] = 1;
    // buffer[1] = 2;
    // buffer[2] = 3;
    // buffer[3] = 4;
    value = analogRead(X_PIN);
    if (value < 1000) 
    {
      buffer[4] = -1;
    } 
    else if (value <= 3000)  // ← Сюда попадут только значения от 1900 до 2100
    {
      buffer[4] = 0;
    } 
    else 
    {
      buffer[4] = 1;
    }
    val = analogRead(Y_PIN);
    if (val < 1900) 
    {
      buffer[5] = -1;
    } 
    else if (val <= 2100)  // ← Сюда попадут только значения от 1900 до 2100
    {
      buffer[5] = 0;
    } 
    else 
    {
      buffer[5] = 1;
    }
    // критическая секция
    xSemaphoreTake(mutex, portMAX_DELAY);
    for (int i=0; i< ARRSIZE; i++)
    {
      arrr1[i]=buffer[i];
    }
    // конец критической секции
    xSemaphoreGive(mutex);
    Serial.println("Ядро 1 активно");
    Serial.println(xPortGetCoreID());
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Задержка 1 секунда
  }
}

void main_task(void *pvParameters){
  while(1) { 
    // критическая секция
    xSemaphoreTake(mutex, portMAX_DELAY);
    for ( int i=0; i< ARRSIZE; i++)
    {
      arrr2[i]=arrr1[i];
    }
    xSemaphoreGive(mutex);
    // конец критической секции // Бесконечный цикл - обязательно!
    Serial.println("Ядро 2 активно");
    Serial.println(arrr2[0]);
    Serial.println(arrr2[1]);
    Serial.println(arrr2[2]);
    Serial.println(arrr2[3]);
    Serial.println(arrr2[4]);
    Serial.println(arrr2[5]);
    Serial.println(xPortGetCoreID());
    Serial.println();
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Задержка 2 секунды
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  // SemaphoreHandle_t mutex;
  mutex = xSemaphoreCreateMutex();
  // memset(&shared, 0, sizeof(SharedData));
  analogReadResolution(12);
  xTaskCreatePinnedToCore(
    input_task,    // Функция задачи
    "InputTask",   // Имя задачи (для отладки)
    2048,          // Размер стека в словах
    NULL,          // Параметры задачи
    1,             // Приоритет
    NULL,          // Дескриптор задачи (не нужен)
    0              // Ядро 0
  );

  xTaskCreatePinnedToCore(
    main_task,     // Функция задачи
    "MainTask",    // Имя задачи
    2048,          // Размер стека
    NULL,          // Параметры
    1,             // Приоритет
    NULL,          // Дескриптор
    1              // Ядро 1
  );

  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}