#pragma once

#include <vector>
#include "window.h"
#include "tanks_g.h"

#define WIDTH  20    // ширина поля (клеток по горизонтали)
#define HEIGHT 8

class tanks_w : public window{
  private:
    const int cellW = 6;                 // ширина символа при textSize=1 (6 пикселей)
    const int cellH = 8;                 // высота символа
    const int cols = 20;
    const int rows = 8;
    // Вычисляем отступы для центровки поля на экране 128x64
    // int offsetX = (display->width()  - cols * cellW) / 2;
    // int offsetY = (display->height() - rows * cellH) / 2;
    int offsetX = 5;
    int offsetY = 5;
    int** field;
    tanks_g* GAME = nullptr;
  public:
    tanks_w();
    ~tanks_w();
    
    bool draw() override;
    bool process_command(std::vector<int> input_v) override;
    bool start_game();
};