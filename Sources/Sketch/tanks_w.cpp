#include "tanks_w.h"

tanks_w::tanks_w() : window(){
  int r = 0;
  lend = new int*[HEIGHT];
  for (int i = 0; i<HEIGHT; i++){
    lend[i] = new int[WIDTH];
  }
  for (int i =0; i<HEIGHT; i++){
    for (int j=0; j<WIDTH; j++){
      r = random(100);
      if (r%3 == 0){
        lend[i][j] = 1;
      } else {
        lend[i][j] = 0;
      }
    }
  }
}

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

  info = GAME->get_info();

  int value = 0;
  char ch;
  int x_0 = std::max(info[0]-cols, 0);
  int y_0 = std::max(info[1]-rows, 0);
  int x_1 = std::max(std::min(info[0]+cols, WIDTH), W_WIDTH);
  int y_1 = std::max(std::min(info[1]+rows, HEIGHT), W_HEIGHT);
  for (int y = y_0; y < y_1; y++) {
        for (int x = x_0; x < x_1; x++) {
  // for (int y = 0; y < rows; y++) {
  //       for (int x = 0; x < cols; x++) {
            value = field[y][x];
            if (value == 0) {
                if (lend[y][x] == 1){ch = '.';}
                else {ch = ' ';}
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
            int screenX = (x-x_0) * cellW;
            int screenY = (y-y_0) * cellH;
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