#include "commander.h"

commander::commander() : current_window_number(0), input_v(6), display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS){
  window_v.push_back(new debugging_w());
  for (int i = 0; i< window_v.size(); i++){
    window_v[i]->set_display(&display);
  }
  SPI.begin(18, 12, 23, OLED_CS);
  display.begin(SSD1306_SWITCHCAPVCC, 0);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  // display.println("TEST");
  // display.display();
  // ABOBA
}

commander::~commander(){}

bool commander::set_input_v(){return true;}

bool commander::process(){
  set_input_v();
  window_v[current_window_number]->process_command(input_v);
  window_v[current_window_number]->draw();
  current_window_number = window_v[current_window_number]->get_next_window_number();
  return true;
}