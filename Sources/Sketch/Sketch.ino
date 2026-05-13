
#define ARRSIZE  6



#include "input_worker.h"
#include "commander.h"
struct TaskResources {
    int* arrr1;             // Указатель на массив в куче
    SemaphoreHandle_t mutex; // Хэндл семафора
};


void input_task(void *pvParameters){
  TaskResources* resources = (TaskResources*)pvParameters;
  input_worker Input_worker;
  while(1) { 
    Input_worker.Reader();
    Input_worker.Semofor(resources->mutex, resources->arrr1);
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}

void main_task(void *pvParameters){
  commander Commander;
  TaskResources* resources = (TaskResources*)pvParameters;
  while(1) {  
    Commander.process(resources->mutex, resources->arrr1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);
  int* arrr1 = new int[ARRSIZE]; 
  SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
  for(int i=0; i<ARRSIZE; i++) arrr1[i] = 0;
  TaskResources* params = new TaskResources();
  params->arrr1 = arrr1;
  params->mutex = mutex;

  xTaskCreatePinnedToCore(
    input_task,    // Функция задачи
    "InputTask",   // Имя задачи (для отладки)
    2048,          // Размер стека в словах
    (void*)params,          // Параметры задачи
    1,             // Приоритет
    NULL,          // Дескриптор задачи (не нужен)
    0              // Ядро 0
  );

  xTaskCreatePinnedToCore(
    main_task,     // Функция задачи
    "MainTask",    // Имя задачи
    2048,          // Размер стека
    (void*)params,          // Параметры
    1,             // Приоритет
    NULL,          // Дескриптор
    1              // Ядро 1
  );

  vTaskDelete(NULL);
}

void loop() {
}
