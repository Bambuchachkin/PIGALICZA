#include "server_w.h"

server_w::server_w(): window(), code_pos(-1), cursor_row(0), cursor_col(0), move_allowed_h(true), move_allowed_v(true), btn_prev(true){
    for (int i = 0; i < 5; ++i) code[i] = '_';
}

void server_w::reset_movement_locks(const std::vector<int>& input_v) {
    if (input_v[0] == 0) move_allowed_h = true;
    if (input_v[1] == 0) move_allowed_v = true;
}

bool server_w::draw() {
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);

    // ---- Code line at the top ----
    display->setCursor(10, 0);
    display->print("CODE: ");
    for (int i = 0; i < 5; ++i) {
        display->print(code[i]);
        display->print(' ');
    }

    // ---- Virtual keyboard ----
    const int startX = 20;   // left margin for the keyboard
    const int startY = 20;   // top of first row
    const int cellW = 18;    // width of each digit cell
    const int cellH = 16;    // height of each row
    const int spacing = 4;   // gap between cells

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 5; ++col) {
            int x = startX + col * (cellW + spacing);
            int y = startY + row * (cellH + 4);
            if (row == cursor_row && col == cursor_col) {
                display->fillRect(x - 2, y - 2, cellW + 4, cellH + 4, SSD1306_WHITE);
                display->setTextColor(SSD1306_BLACK);
            } else {
                display->setTextColor(SSD1306_WHITE);
            }

            display->setCursor(x, y);
            display->setTextSize(1);
            display->print(keyboard[row][col]);
        }
    }

    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 54);
    display->print("Nav:joyst.  Btn:enter");

    display->display();
    return true;
}

bool server_w::process_command(std::vector<int> input_v) {
    reset_movement_locks(input_v);

    if (input_v[0] == -1 && move_allowed_h) {   // left
        move_allowed_h = false;
        if (cursor_col > 0) --cursor_col;
    }
    else if (input_v[0] == 1 && move_allowed_h) { // right
        move_allowed_h = false;
        if (cursor_col < 4) ++cursor_col;
    }
    if (input_v[1] == -1 && move_allowed_v) {   // down
        move_allowed_v = false;
        if (cursor_row < 1) ++cursor_row;
    }
    else if (input_v[1] == 1 && move_allowed_v) { // up
        move_allowed_v = false;
        if (cursor_row > 0) --cursor_row;
    }

    bool btn_current = (input_v[2] == 1);
    if (btn_current && !btn_prev) {
      // if (code_pos == -1) return true;
      if (code_pos < 5){
        code[code_pos++] = keyboard[cursor_row][cursor_col];
      } else {
        code_pos = 0;
        for (int i = 0; i < 5; ++i) code[i] = '_';
      }
    }
    btn_prev = btn_current;

    if (input_v[3] == 1) {
        code_pos = -1;
        next_window_number = 1;
    }

    return true;
}