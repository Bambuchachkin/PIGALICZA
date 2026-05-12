#pragma once

#include <vector>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "window.h"
#include "debugging_w.h"
#include "menu_w.h"
#include "tanks_w.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_DC     16
#define OLED_RST    17
#define OLED_CS     19

#define X_PIN 33
#define Y_PIN 32
#define BUTTON_PIN_1 26 //25 acept
#define BUTTON_PIN_2 25 //26 action
#define BUTTON_PIN_3 21 //21 reject
#define BUTTON_PIN_4 22 //22 -

class commander{
  private:
    Adafruit_SSD1306 display;
    std::vector<int> input_v;
    std::vector<window*> window_v;
    int current_window_number;

    int x = 1700;
    int y = 1700;
  public:
    commander();
    ~commander();

    bool set_input_v();
    bool process();
};