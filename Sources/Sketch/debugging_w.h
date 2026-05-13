#pragma once

#include <vector>
#include "window.h"

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
