void input_task(void *pvParameters){
  while(1) {  // Бесконечный цикл - обязательно!
    Serial.println("Ядро 1 активно");
    Serial.println(xPortGetCoreID());
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Задержка 1 секунда
  }
}

void main_task(void *pvParameters){
  while(1) {  // Бесконечный цикл - обязательно!
    Serial.println("Ядро 2 активно");
    Serial.println(xPortGetCoreID());
    Serial.println();
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Задержка 2 секунды
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);

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
