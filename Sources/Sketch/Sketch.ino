#define ARRSIZE 6
#include "input_worker.h"
#include "commander.h"

#define buttonPin 22

//дескрипторы задач
TaskHandle_t inputTaskHandle = NULL;
TaskHandle_t mainTaskHandle = NULL;
TaskHandle_t pauseManagerHandle = NULL;

volatile bool pauseRequest = false; //флаг прерывания
volatile uint32_t lastInterruptTime = 0;
const uint32_t debounceDelayUs = 200000;   // 200 мс
bool tasksRunning = true;

//обработчик прерывания + антидребезг
void IRAM_ATTR buttonISR() { //IRAM_ATTR — атрибут, указывающий, что функция должна быть размещена в оперативной памяти (IRAM), а не во flash.
    uint32_t now = esp_timer_get_time();
    if ((now - lastInterruptTime) < debounceDelayUs) {
        return;
    }
    lastInterruptTime = now;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // флаг, указывающий, требуется ли переключение контекста после ISR.
    if (pauseManagerHandle) {
        vTaskNotifyGiveFromISR(pauseManagerHandle, &xHigherPriorityTaskWoken);
    }
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //макрос FreeRTOS, который запрашивает переключение контекста после завершения ISR.
    }
}

//сложная задача для прерывания
void pause_manager_task(void *pvParameters) {
    while (1) {
        //ожидание уведомления от ISR
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (tasksRunning) {
            if (inputTaskHandle) vTaskSuspend(inputTaskHandle);
            if (mainTaskHandle) vTaskSuspend(mainTaskHandle);
            tasksRunning = false;
            Serial.println("PAUSE ON");
        } else {
            if (inputTaskHandle) vTaskResume(inputTaskHandle);
            if (mainTaskHandle) vTaskResume(mainTaskHandle);
            tasksRunning = true;
            Serial.println("PAUSE OFF");
        }
    }
}


struct TaskResources {
    int* arrr1;
    SemaphoreHandle_t mutex;
};

void input_task(void *pvParameters) {
    TaskResources* resources = (TaskResources*)pvParameters;
    input_worker Input_worker;
    while (1) {
        Input_worker.Reader();
        Input_worker.Semofor(resources->mutex, resources->arrr1);
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

void main_task(void *pvParameters) {
    commander Commander;
    TaskResources* resources = (TaskResources*)pvParameters;
    while (1) {
        Commander.process(resources->mutex, resources->arrr1);
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

void setup() {
    Serial.begin(9600);
    // Выделение ресурсов
    int* arrr1 = new int[ARRSIZE];
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    for (int i = 0; i < ARRSIZE; i++) arrr1[i] = 0;

    TaskResources* params = new TaskResources();
    params->arrr1 = arrr1;
    params->mutex = mutex;

    // Создание задач
    xTaskCreatePinnedToCore(input_task, "InputTask", 2048, (void*)params, 1, &inputTaskHandle, 0);
    xTaskCreatePinnedToCore(main_task, "MainTask", 2048, (void*)params, 1, &mainTaskHandle, 1);
    xTaskCreatePinnedToCore(pause_manager_task, "PauseManager", 1024, NULL, 2, &pauseManagerHandle, 0);

    // Настройка кнопки
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);

    vTaskDelete(NULL);
}

void loop() {
}