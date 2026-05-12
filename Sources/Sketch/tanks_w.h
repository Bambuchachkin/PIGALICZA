#pragma once

#include <vector>
#include "window.h"
#include "tanks_g.h"

#define WIDTH  42 // полная ширина окна
#define HEIGHT 16
#define SCALE 1
#define W_WIDTH 21 // ширина отображаемого окна
#define W_HEIGHT 8

class tanks_w : public window{
  private:
    const int cellW = 6*SCALE;                 // ширина символа при textSize=1 (6 пикселей)
    const int cellH = 8*SCALE;                 // высота символа
    const int cols = W_WIDTH/SCALE/2;
    const int rows = W_HEIGHT/SCALE/2;
    int** field;
    int** lend;
    tanks_g* GAME = nullptr;
    std::vector<int> info;
  public:
    tanks_w();
    ~tanks_w();
    
    bool draw() override;
    bool process_command(std::vector<int> input_v) override;
    bool start_game();
};