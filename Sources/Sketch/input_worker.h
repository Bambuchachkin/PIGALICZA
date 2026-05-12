#ifndef INPUT_WORKER_H
#define INPUT_WORKER_H
#include <cstring>
#include <Arduino.h>  
#define ARRSIZE  6
#define X_PIN  33
#define Y_PIN  32
#define BUTTON_PIN_1  25
#define BUTTON_PIN_2  26
#define BUTTON_PIN_3  21
#define BUTTON_PIN_4  22
class input_worker{
  // private:
  // int buffer[ARRSIZE];
  public:
  int buffer[ARRSIZE];
  input_worker();
  ~input_worker();
  void Reader();
  void Semofor(SemaphoreHandle_t mutex, int* arrr1);
};

#endif
