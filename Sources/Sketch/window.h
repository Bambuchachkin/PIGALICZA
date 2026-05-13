#pragma once

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
    // unsigned long timer;
    unsigned long record_time;
    int record_score;
    int score;
    std::vector<unsigned long> record_times_win;
    std::vector<unsigned long> record_scores_win;
    int get_next_window_number();
    bool set_next_window_number(int num);
    bool set_display(Adafruit_SSD1306* disp);
    virtual bool draw();
    virtual bool process_command(std::vector<int> input_v);
};