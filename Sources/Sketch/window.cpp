#include "window.h"

window::window() : next_window_number(0){}

window::~window(){}

int window::get_next_window_number(){
  return next_window_number;
}

bool window::set_next_window_number(int num){
  next_window_number = num;
  return true;
}

bool window::set_display(Adafruit_SSD1306* disp){
  display = disp;
  return true;
}

bool  window::draw(){return true;}
bool  window::process_command(std::vector<int> input_v){return true;}