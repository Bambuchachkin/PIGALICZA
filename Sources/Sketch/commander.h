#pragma once

#include <vector>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "window.h"
#include "tanks_w.h"
#include "debugging_w.h"
#include "menu_w.h"
#include "record_w.h"
#define ARRSIZE  6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC     16
#define OLED_RST    17
#define OLED_CS     19

class commander{
  private:
    Adafruit_SSD1306 display;
    std::vector<window*> window_v;
    int current_window_number;

    int x = 1700;
    int y = 1700;
  public:
    unsigned long mill;
    std::vector<int> input_v;
    commander();
    ~commander();

    bool set_input_v(SemaphoreHandle_t mutex, int* arrr1);
    bool process(SemaphoreHandle_t mutex, int* arrr1);
};