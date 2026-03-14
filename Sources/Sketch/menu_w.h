#ifndef MENU_W_H
#define MENU_W_H

#include <vector>
// #include <string>
#include "window.h"


class menu_w : public window{
  private:
    // std::vector<std::string> items_v;
    std::vector<const char*> items_v;
    int selected_item_num;
    int scroll_offset;
  public:
    menu_w();
    ~menu_w();
    
    bool draw() override;
    bool process_command(std::vector<int> input_v) override;
};

#endif