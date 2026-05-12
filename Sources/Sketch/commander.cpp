#include "commander.h"

commander::commander() : current_window_number(1), input_v(6), display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS){
  SPI.begin(18, 12, 23, OLED_CS);
  display.begin(SSD1306_SWITCHCAPVCC, 0);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
}

commander::~commander(){}

bool commander::set_input_v(SemaphoreHandle_t mutex, int* arrr1){
  xSemaphoreTake(mutex, portMAX_DELAY);
    for (int i=0; i< ARRSIZE; i++)
    {
      input_v[i]=arrr1[i];
    }
    // конец критической секции
    xSemaphoreGive(mutex);
  return true;
}

bool commander::process(SemaphoreHandle_t mutex, int* arrr1){
  set_input_v(mutex, arrr1);
  window_v[current_window_number]->set_next_window_number(current_window_number);
  window_v[current_window_number]->process_command(input_v);
  window_v[current_window_number]->draw();
  current_window_number = window_v[current_window_number]->get_next_window_number();
  return true;
}