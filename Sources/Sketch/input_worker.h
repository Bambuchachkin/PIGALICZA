#ifndef INPUT_WORKER_H
#define INPUT_WORKER_H
#include <cstring>
#define ARRSIZE  6
#define X_PIN  32;
#define Y_PIN  33;
#define BUTTON_PIN_1 25
#define BUTTON_PIN_2  26
#define BUTTON_PIN_3 35
#define BUTTON_PIN_4 34
class input_worker{
  private:
  int buffer[ARRSIZE];
  public:
  SemaphoreHandle_t mutex;
  int arrr1[ARRSIZE];
  input_worker();
  ~input_worker();
  void Reader();
  void Semofor();
}

#endif