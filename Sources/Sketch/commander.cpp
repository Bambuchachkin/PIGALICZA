#include "commander.h"

commander::commander() : current_window_number(1), input_v(6), display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS){
  window_v.push_back(new debugging_w());
  // window_v.push_back(new debugging_w());
  window_v.push_back(new menu_w());
  for (int i = 0; i< window_v.size(); i++){
    window_v[i]->set_display(&display);
  }
  SPI.begin(18, 12, 23, OLED_CS);
  display.begin(SSD1306_SWITCHCAPVCC, 0);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);


  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  analogReadResolution(12);
}

commander::~commander(){}

bool commander::set_input_v(){
  int x = analogRead(X_PIN);
  int y = analogRead(Y_PIN);
  if (x>3000){
    input_v[0] = 1;
  } else if (x<1000){
    input_v[0] = -1;
  } else {
    input_v[0] = 0;
  }
  if (y>3000){
    input_v[1] = 1;
  } else if (y<1000){
    input_v[1] = -1;
  } else {
    input_v[1] = 0;
  }
  input_v[2] = digitalRead(BUTTON_PIN_1);
  input_v[3] = digitalRead(BUTTON_PIN_2);
  input_v[4] = digitalRead(BUTTON_PIN_3);
  input_v[5] = digitalRead(BUTTON_PIN_4);
  return true;
}

bool commander::process(){
  set_input_v();
  window_v[current_window_number]->set_next_window_number(current_window_number);
  window_v[current_window_number]->process_command(input_v);
  window_v[current_window_number]->draw();
  current_window_number = window_v[current_window_number]->get_next_window_number();
  return true;
}