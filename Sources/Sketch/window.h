#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

class window{
  protected:
    int next_window_number;
    Adafruit_SSD1306* display;
  public:
    window();
    virtual ~window();

    int get_next_window_number();
    bool set_next_window_number(int num);
    bool set_display(Adafruit_SSD1306* disp);
    
    virtual bool draw();
    virtual bool process_command(std::vector<int> input_v);
};

#endif