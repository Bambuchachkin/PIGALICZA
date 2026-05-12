#include "input_worker.h"
input_worker::input_worker()
{
  memset(buffer, 0, ARRSIZE);
  // buffer[0] = 1;
  // buffer[1] = 1;
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  analogReadResolution(12);
}
input_worker::~input_worker(){}
void input_worker::Reader()
{
    buffer[0] = !(digitalRead(BUTTON_PIN_1));
    buffer[1] = !(digitalRead(BUTTON_PIN_2));
    buffer[2] = !(digitalRead(BUTTON_PIN_3));
    buffer[3] = !(digitalRead(BUTTON_PIN_4));
    value = analogRead(X_PIN);
    if (value < 1000) 
    {
      buffer[4] = -1;
    } 
    else if (value <= 3000)  
    {
      buffer[4] = 0;
    } 
    else 
    {
      buffer[4] = 1;
    }
    val = analogRead(Y_PIN);
    if (val < 1000) 
    {
      buffer[5] = -1;
    } 
    else if (val <= 3000)  
    {
      buffer[5] = 0;
    } 
    else 
    {
      buffer[5] = 1;
    }
}
void input_worker::Semofor(SemaphoreHandle_t mutex, int* arrr1)
{
  xSemaphoreTake(mutex, portMAX_DELAY);
    for (int i=0; i< ARRSIZE; i++)
    {
      arrr1[i]=buffer[i];
    }
    // конец критической секции
    xSemaphoreGive(mutex);
}
