#include "input_worker.h"
input_worker::input_worker()
{
  memset(buffer, 0, ARRSIZE);
  memset(arrr1, 0, ARRSIZE);
  arrr1[0] = 1;
  arrr1[1] = 1;
  buffer[0] = 1;
  buffer[1] = 1;
  mutex = xSemaphoreCreateMutex();
}
input_worker::~input_worker(){}
void input_worker::Reader()
{
    analogReadResolution(12);
    
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
void input_worker::Semofor()
{
  xSemaphoreTake(mutex, portMAX_DELAY);
    for (int i=0; i< ARRSIZE; i++)
    {
      arrr1[i]=buffer[i];
    }
    // конец критической секции
    xSemaphoreGive(mutex);
}
