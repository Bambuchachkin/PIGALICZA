#include "debugging_w.h"

debugging_w::debugging_w() : window()/*, record_times_win(3)*/{}

debugging_w::~debugging_w(){};

bool debugging_w::draw(){
  display->clearDisplay();
  display->setCursor(0, 0);
  display->print("Joystick_x = ");
  display->print(x);
  display->print("\n");
  display->print("Joystick_y = ");
  display->print(y);
  display->print("\n");
  display->print("btn_1 = ");
  display->print(btn_1);
  display->print("\n");
  display->print("btn_2 = ");
  display->print(btn_2);
  display->print("\n");
  display->print("btn_3 = ");
  display->print(btn_3);
  display->print("\n");
  display->print("btn_4 = ");
  display->print(btn_4);
  display->print("\n");
  display->print("record time in tanks:");
  display->print(record_times_win[2]);
  display->display();

  Serial.println("Records");
  for (int i =0; i<3; i++){
    if (record_times_win[i]!=0){
      Serial.println(String(record_times_win[i]));
    }
  }
  return true;
}

bool debugging_w::process_command(std::vector<int> input_v){
  x = input_v[0];
  y = input_v[1];
  btn_1 = input_v[2];
  btn_2 = input_v[3];
  btn_3 = input_v[4];
  btn_4 = input_v[5];

  if (input_v[3] == 1){
    next_window_number = 1;
    return true;
  }

  return true;
}