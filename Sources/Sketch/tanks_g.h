#pragma once

#include <stdlib.h>
#include <vector>

// ==================== Настройки ====================
#define BOT_COUNT 8
#define BULLET_COUNT 10
#define PAUSE 3
#define TERMINATE_BUTTON 3

struct Player {
    int x_position;   // по горизонтали (0..WIDTH-1)
    int y_position;   // по вертикали   (0..HEIGHT-1)
    int angle;
    int width;
    int** body;
    int owner = 0;
};

struct Bullet {
    int x_position;
    int y_position;
    int Vector;
    int owner = 0;
};

class tanks_g {
  private:
    int WIDTH = 0;
    int HEIGHT = 0;
    int BTN_UP = 0;
    int BTN_DOWN = 0;
    int BTN_LEFT = 0;
    int BTN_RIGHT = 0;
    int BTN_FIRE = 0;

    Player User;
    int score = 0;
    int record = 0;
    int** Field_Matrix = nullptr;
    int** Body_Matrix = nullptr;

    Bullet* Bullets[BULLET_COUNT] = {};
    Player* Bots[BOT_COUNT] = {};

    int killCode = 0;

    int timer = PAUSE;

    bool InBounds(int x, int y) const;
    bool IsWallCell(int x, int y) const;
    int Clamp(int value, int minValue, int maxValue) const;
    void FreeMatrix(int** matrix);
    void ResetResources();
    void ResetBullet(Bullet* bullet, int** Field = nullptr);
    void ResetField(int** Field);
    void ClearTankFromField(Player* user, int** Field);
    void RemoveBot(int index, int** Field);
    bool ApplyKillCode(int code, int** Field);

  public:
    tanks_g(int w, int h);
    ~tanks_g();
    bool start_game();
    bool continue_game();
    int** get_field();

    int** Field_Init();
    int** Body_Init(Player* user);
    bool Place_Check(int** Field, int sx, int sy);
    void Tank_Print(Player* user, int** Field);
    bool Body_Update(int Vector, Player* user, int** Field);
    bool Shooting(Player* user, int** Field, Bullet* bullets[]);
    bool Bullet_Update(Bullet* bullets[], int** Field);
    void Bot_Init(Player* bots[], int number);
    void Bot_Update(Player* bots[], int** Field, Bullet* bullets[]);
    int Distance(Player* a, Player* b);
    int Distance_B(Player* p, Bullet* b);
    int Kill(Player* user, Player* bots[], int** Field, Bullet* bullets[]);
    void Real_Debugging(Player* bots[], Bullet* bullets[], Player* user, int** Field);
    int GetButtonCommand();
    bool game_continue();
    void set_buttons(const std::vector<int>& buttons);
    void zeroing();
    std::vector<int> get_info(); // player_x, player_y, score, time
};
