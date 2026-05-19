#include "menu_w.h"

menu_w::menu_w() : window(), selected_item_num(0), scroll_offset(0), scroll_available(true) {
  Serial.println("=== menu_w() ===");
  items_v.push_back("Debug");
  items_v.push_back("Tanks");
  items_v.push_back("Records");
  items_v.push_back("Auto-tion");
}

menu_w::~menu_w(){}

bool menu_w::draw(){
  display->clearDisplay();
  
  // Заголовок
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);
  display->println("GAMES");
  
  // Сколько пунктов помещается на экране
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
    
    // Название игры крупным шрифтом (без смещения)
    display->setCursor(20, yPos);
    display->setTextSize(2);
    display->println(items_v[item_index]);
    display->setTextSize(1);
  }
  
  // Подсказки внизу
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 54);
  display->print("d:Play u:Esc r:pause");
  
  display->display();
  return true;
}

bool menu_w::process_command(std::vector<int> input_v){
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
  if(input_v[2] == 1) {
    switch(selected_item_num){
      case 0: 
        next_window_number = 0;
        break;
      case 1:
        next_window_number = 2;
        break;
      case 2:
        next_window_number = 3;
        break;
      case 3:
        next_window_number = 4;
        break;
    }
  }
  
  return true;
}