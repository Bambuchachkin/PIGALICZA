#include "record_w.h"

record_w::record_w() :  window(), selected_item_num(0), scroll_offset(0), scroll_available(true) {
  items_v.push_back("Debug");
  items_v.push_back("Tanks");
}

record_w::~record_w(){}

bool record_w::draw(){
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
  display->println("RECORDS:");
  
  int max_visible = 2;
  int lineHeight = 22; // Фиксированная высота строки
  
  // Рисуем видимые пункты меню крупным шрифтом
  for(int i = 0; i < max_visible && (scroll_offset + i) < (int)items_v.size(); i++) {
    int item_index = scroll_offset + i;
    int yPos = 12 + i * lineHeight; // Уменьшил начальный отступ для равномерности
    
    if(item_index == selected_item_num) {
      // Выбранный пункт - белый фон
      display->fillRect(0, yPos-2, display->width(), 18, SSD1306_WHITE);
      display->setTextColor(SSD1306_BLACK);
    } else {
      display->setTextColor(SSD1306_WHITE);
    }

    int aboba = 0;
    if (item_index == 1){
      aboba = 2;
    }
    
    // Название игры крупным шрифтом (без смещения)
    display->setCursor(20, yPos);
    display->setTextSize(2);
    display->print(items_v[item_index]);
    display->print(":");
    display->print(record_times_win[aboba]);
    display->setTextSize(1);
  }

  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 54);
  display->print("up:Esc");

  display->display();
  return true;
}

bool record_w::process_command(std::vector<int> input_v){
  int max_visible = 2;
  int total_items = items_v.size();
  
  // Навигация по джойстику
  if(input_v[1] == -1 && scroll_available) { // Вниз
    scroll_available = false;
    if(selected_item_num < total_items - 1) {
      selected_item_num++;
      // Пролистываем вниз если нужно
      if(selected_item_num >= scroll_offset + max_visible) {
        scroll_offset++;
      }
    }
  } else if(input_v[1] == 1 && scroll_available) { // Вверх
    scroll_available = false;
    if(selected_item_num > 0) {
      selected_item_num--;
      // Пролистываем вверх если нужно
      if(selected_item_num < scroll_offset) {
        scroll_offset--;
      }
    }
  } else if(input_v[1] == 0){
    scroll_available = true;
  }
  
  // Кнопки выбора
  if(input_v[3] == 1) { // BTN3 - Play
    next_window_number = 1;
  }
  
  return true;
}