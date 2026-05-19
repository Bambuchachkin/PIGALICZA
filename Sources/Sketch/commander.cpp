#include "commander.h"

commander::commander() : current_window_number(1), input_v(6), display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS){
  SPI.begin(18, 12, 23, OLED_CS);
  display.begin(SSD1306_SWITCHCAPVCC, 0);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  window_v.push_back(new debugging_w());
  window_v.push_back(new menu_w());
  window_v.push_back(new tanks_w());
  window_v.push_back(new record_w());
  window_v.push_back(new server_w());

  for (int i = 0; i< window_v.size(); i++){
    window_v[i]->set_display(&display);
  }
  mill= millis();
}
commander::~commander(){}

bool commander::set_input_v(SemaphoreHandle_t mutex, int* arrr1){
  xSemaphoreTake(mutex, portMAX_DELAY);
    for (int i=0; i< ARRSIZE; i++)
    {
      input_v[i]=arrr1[i];
      arrr1[i]=0;
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
  if (current_window_number != (window_v[current_window_number]->get_next_window_number()))
  {
    unsigned long timer=millis();
    if ((timer-mill) > window_v[current_window_number]->record_time)
    {
      window_v[current_window_number]->record_time = timer-mill; 
      window_v[3]->record_times_win[current_window_number] = (timer-mill)/1000;
      Serial.println("TIMER-MIL:");
      Serial.println(String(timer-mill));
      Serial.println(String(window_v[3]->record_times_win[current_window_number]));
    }
    mill = timer;

    if (window_v[current_window_number]->record_score < window_v[current_window_number]->score)
    {
      window_v[current_window_number]->record_score =  window_v[current_window_number]->score; 
      window_v[3]->record_scores_win[current_window_number] = window_v[current_window_number]->score;
    }
  }
  current_window_number = window_v[current_window_number]->get_next_window_number();
  return true;
}