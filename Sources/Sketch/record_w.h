#pragma once

#include <vector>
#include "window.h"


class record_w : public window{
  private:
    std::vector<const char*> items_v;
    int selected_item_num;
    int scroll_offset;
    bool scroll_available;
  public:
    record_w();
    ~record_w();
    
    bool draw() override;
    bool process_command(std::vector<int> input_v) override;
};