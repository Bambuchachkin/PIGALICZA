#include "debugging_w.h"

debugging_w::debugging_w() : window() {
  // pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  // pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  // pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  // pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  // analogReadResolution(12);
}

debugging_w::~debugging_w(){};

bool debugging_w::draw(){
  // x = analogRead(X_PIN);
  // y = analogRead(Y_PIN);
  // btn_1 = digitalRead(BUTTON_PIN_1);
  // btn_2 = digitalRead(BUTTON_PIN_2);
  // btn_3 = digitalRead(BUTTON_PIN_3);
  // btn_4 = digitalRead(BUTTON_PIN_4);

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
  display->display();
  return true;
}

bool debugging_w::process_command(std::vector<int> input_v){
  x = input_v[0];
  y = input_v[1];
  btn_1 = input_v[2];
  btn_2 = input_v[3];
  btn_3 = input_v[4];
  btn_4 = input_v[5];

  if (input_v[3] == 0){
    next_window_number = 1;
      return true;
  }

  return true;
}