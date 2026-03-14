#ifndef DEBUGGING_W_H
#define DEBUGGING_W_H

#include <vector>
#include "window.h"

// const int X_PIN = 32;
// const int Y_PIN = 33;
// const int BUTTON_PIN_1 = 25;
// const int BUTTON_PIN_2 = 26;
// const int BUTTON_PIN_3 = 35;
// const int BUTTON_PIN_4 = 34;

class debugging_w : public window{
  private:
    int x = 0;
    int y = 0;
    int btn_1 = 0;
    int btn_2 = 0;
    int btn_3 = 0;
    int btn_4 = 0;
  public:
    debugging_w();
    ~debugging_w();
    
    bool draw() override;
    bool process_command(std::vector<int> input_v) override;
};

#endif