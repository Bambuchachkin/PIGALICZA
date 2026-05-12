#ifndef INPUT_WORKER_H
#define INPUT_WORKER_H
#include <cstring>
#define ARRSIZE  6
const int X_PIN = 32;
const int Y_PIN = 33;
const int BUTTON_PIN_1 = 25;
const int BUTTON_PIN_2 = 26;
const int BUTTON_PIN_3 = 21;
const int BUTTON_PIN_4 = 22;
class input_worker{
  private:
  int buffer[ARRSIZE];
  public:
  input_worker();
  ~input_worker();
  void Reader();
  void Semofor(SemaphoreHandle_t mutex, int (&arrr1)[ARRSIZE]);
}

#endif
