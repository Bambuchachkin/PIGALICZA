#include "input_worker.h"
input_worker::input_worker()
{
  for (int i = 0; i < ARRSIZE; i++) buffer[i] = 0;
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  analogReadResolution(12);
}
input_worker::~input_worker(){}
void input_worker::Reader()
{
    buffer[2] = !(digitalRead(BUTTON_PIN_1));
    buffer[3] = !(digitalRead(BUTTON_PIN_2));
    buffer[4] = !(digitalRead(BUTTON_PIN_3));
    buffer[5] = !(digitalRead(BUTTON_PIN_4));
    int value = analogRead(X_PIN);
    if (value < 1000) 
    {
      buffer[0] = 1;
    } 
    else if (value <= 2500)  
    {
      buffer[0] = 0;
    } 
    else 
    {
      buffer[0] = -1;
    }
    int val = analogRead(Y_PIN);
    if (val < 1000) 
    {
      buffer[1] = 1;
    } 
    else if (val <= 2500)  
    {
      buffer[1] = 0;
    } 
    else 
    {
      buffer[1] = -1;
    }
}
void input_worker::Semofor(SemaphoreHandle_t mutex, int* arrr1)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
    // bool allZero = true;
    // for (int i = 0; i < ARRSIZE; i++) {
    //     if (buffer[i] != 0) {
    //         allZero = false;
    //         break;
    //     }
    // }
    // if (allZero) {
    //     delay(100);
    // }
    for (int i=0; i< ARRSIZE; i++)
    {
      arrr1[i]=buffer[i];
    }
    // конец критической секции
    xSemaphoreGive(mutex);
}
