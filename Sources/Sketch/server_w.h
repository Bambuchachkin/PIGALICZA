#pragma once

#include <vector>
#include "window.h"

class server_w : public window {
private:
    static constexpr char keyboard[2][5] = {
        {'0', '1', '2', '3', '4'},
        {'5', '6', '7', '8', '9'}
    };
    char code[5];
    uint8_t code_pos;
    uint8_t cursor_row;
    uint8_t cursor_col;

    bool move_allowed_h;
    bool move_allowed_v;
    bool btn_prev;
    void reset_movement_locks(const std::vector<int>& input_v);

public:
    server_w();
    ~server_w() {}

    bool draw() override;
    bool process_command(std::vector<int> input_v) override;
    const char* get_code() const { return code; }
};