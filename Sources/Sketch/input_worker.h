#ifndef INPUT_WORKER_H
#define INPUT_WORKER_H
#include <cstring>
#include <Arduino.h>  
#define ARRSIZE  6
#define X_PIN  33
#define Y_PIN  32
#define BUTTON_PIN_1  26
#define BUTTON_PIN_2  21
#define BUTTON_PIN_3  25
#define BUTTON_PIN_4  22
class input_worker{
  public:
  int buffer[ARRSIZE];
  input_worker();
  ~input_worker();
  void Reader();
  void Semofor(SemaphoreHandle_t mutex, int* arrr1);
};

#endif
