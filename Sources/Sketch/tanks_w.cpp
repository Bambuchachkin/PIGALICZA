#include "tanks_w.h"

tanks_w::tanks_w() : window(){/*Serial.println("=== tanks_w() ===");*/}

tanks_w::~tanks_w(){}

bool tanks_w::draw(){  
  if (GAME == nullptr){
    start_game();
  }
  if (!GAME->game_continue()){
    next_window_number = 1;
    delete GAME;
    GAME=nullptr;
    return true;
  }
  GAME->continue_game();

  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);

  int value = 0;
  char ch;
  for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            value = field[y][x];
            if (value == 0) {
                ch = '.';          // пусто
            } else if (value == 1) {
                ch = '#';          // стена
            } else if (value == 2) {
                ch = '*';          // пуля
            } else {
                // Любое другое число (3..9 и т.д.) выводим как цифру
                if (value >= 0 && value <= 9)
                    ch = '0' + value;
                else
                    ch = '?';
            }
            int screenX = offsetX + x * cellW;
            int screenY = offsetY + y * cellH;
            display->setCursor(screenX, screenY);
            display->print(String(ch));
        }
  }
  display->display();
  // Serial.println("=== tanks_w finish drawing ===");
  return true;
}

bool tanks_w::process_command(std::vector<int> input_v){
  if (GAME!=nullptr){
    GAME->set_buttons(input_v);
  }
  return true;
}

bool tanks_w::start_game(){
  // Serial.println("=== tanks_w::start_game() ===");
  GAME = new tanks_g(WIDTH, HEIGHT);
  GAME->start_game();
  field = GAME->get_field();
  // Serial.println("=== tanks_w get field success ===");
  return true;
}