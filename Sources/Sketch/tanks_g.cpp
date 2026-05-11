#include "tanks_g.h"
#include <cstdlib>
#include <Arduino.h>

tanks_g::tanks_g(int w, int h) : WIDTH(w), HEIGHT(h) {
    randomSeed(analogRead(0));
}

tanks_g::~tanks_g() {
    ResetResources();
}

bool tanks_g::InBounds(int x, int y) const {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

bool tanks_g::IsWallCell(int x, int y) const {
    return !InBounds(x, y) || x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1;
}

int tanks_g::Clamp(int value, int minValue, int maxValue) const {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

void tanks_g::FreeMatrix(int** matrix) {
    if (matrix == nullptr) return;

    for (int i = 0; i < HEIGHT; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void tanks_g::ResetResources() {
    FreeMatrix(Body_Matrix);
    Body_Matrix = nullptr;
    User.body = nullptr;

    for (int i = 0; i < BOT_COUNT; i++) {
        if (Bots[i] != nullptr) {
            FreeMatrix(Bots[i]->body);
            Bots[i]->body = nullptr;
            delete Bots[i];
            Bots[i] = nullptr;
        }
    }

    for (int i = 0; i < BULLET_COUNT; i++) {
        delete Bullets[i];
        Bullets[i] = nullptr;
    }

    FreeMatrix(Field_Matrix);
    Field_Matrix = nullptr;
}

void tanks_g::ResetBullet(Bullet* bullet, int** Field) {
    if (bullet == nullptr) return;

    if (Field != nullptr && InBounds(bullet->x_position, bullet->y_position) && Field[bullet->y_position][bullet->x_position] > 0) {
        Field[bullet->y_position][bullet->x_position]--;
    }

    bullet->Vector = 0;
    bullet->x_position = -1;
    bullet->y_position = -1;
    bullet->owner = 0;
}

void tanks_g::ResetField(int** Field) {
    if (Field == nullptr) return;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Field[y][x] = IsWallCell(x, y) ? 1 : 0;
        }
    }
}

void tanks_g::ClearTankFromField(Player* user, int** Field) {
    if (user == nullptr || Field == nullptr || user->x_position < 0) return;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int x = user->x_position + dx;
            int y = user->y_position + dy;
            if (InBounds(x, y) && !IsWallCell(x, y)) {
                Field[y][x] = 0;
            }
        }
    }
}

void tanks_g::RemoveBot(int index, int** Field) {
    if (index < 0 || index >= BOT_COUNT || Bots[index] == nullptr) return;

    ClearTankFromField(Bots[index], Field);
    Bots[index]->x_position = -2;
    Bots[index]->y_position = -2;
}

bool tanks_g::ApplyKillCode(int code, int** Field) {
    killCode = code;

    if (killCode == -1) {
        Real_Debugging(Bots, Bullets, &User, Field);
        return false;
    }

    if (killCode >= 0 && killCode < BOT_COUNT) {
        score++;
        RemoveBot(killCode, Field);
    }

    return true;
}

int** tanks_g::Field_Init() {
    int** Field = new int*[HEIGHT];
    for (int i = 0; i < HEIGHT; i++) {
        Field[i] = new int[WIDTH];
    }

    ResetField(Field);
    return Field;
}

int** tanks_g::Body_Init(Player* user) {
    int** Body = new int*[HEIGHT];
    for (int i = 0; i < HEIGHT; i++) {
        Body[i] = new int[WIDTH];
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Body[y][x] = 0;
        }
    }

    if (user != nullptr) {
        user->x_position = Clamp((WIDTH - 1) / 2, 1, WIDTH - 2);
        user->y_position = Clamp((HEIGHT - 1) / 2, 1, HEIGHT - 2);
        user->angle = 1;
        user->width = 3;
    }

    return Body;
}

bool tanks_g::Place_Check(int** Field, int S_x, int S_y) {
    if (Field == nullptr) return false;

    for (int y = S_y - 1; y <= S_y + 1; y++) {
        for (int x = S_x - 1; x <= S_x + 1; x++) {
            if (!InBounds(x, y) || IsWallCell(x, y) || Field[y][x] != 0) {
                return false;
            }
        }
    }

    return true;
}

void tanks_g::Tank_Print(Player* user, int** Field) {
    if (user == nullptr || Field == nullptr || user->body == nullptr) return;

    int** Body = user->body;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Body[y][x] = 0;
        }
    }

    if (user->x_position < 0 || user->y_position < 0) return;

    const int up[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {1, 0, 1}
    };
    const int down[3][3] = {
        {1, 0, 1},
        {1, 1, 1},
        {0, 1, 0}
    };
    const int right[3][3] = {
        {1, 1, 0},
        {0, 1, 1},
        {1, 1, 0}
    };
    const int left[3][3] = {
        {0, 1, 1},
        {1, 1, 0},
        {0, 1, 1}
    };

    const int (*shape)[3] = down;
    if (user->angle == -1) shape = up;
    else if (user->angle == 1) shape = down;
    else if (user->angle == 2) shape = right;
    else if (user->angle == -2) shape = left;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int x = user->x_position + dx;
            int y = user->y_position + dy;
            int value = shape[dy + 1][dx + 1];
            if (value == 1 && InBounds(x, y)) {
                Body[y][x] = 1;
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (Body[y][x] == 1) {
                Field[y][x]++;
            }
        }
    }
}

bool tanks_g::Body_Update(int Vector, Player* user, int** Field) {
    if (user == nullptr || Field == nullptr || user->x_position < 0) return false;

    if (Vector == 0) return true;

    int newX = user->x_position;
    int newY = user->y_position;

    if (Vector == 1 || Vector == -1) {
        newY += Vector;
    } else if (Vector == 2 || Vector == -2) {
        newX += (Vector == 2) ? 1 : -1;
    } else {
        return false;
    }

    user->angle = Vector;

    int oldX = user->x_position;
    int oldY = user->y_position;

    ClearTankFromField(user, Field);
    if (Place_Check(Field, newX, newY)) {
        user->x_position = newX;
        user->y_position = newY;
    } else {
        user->x_position = oldX;
        user->y_position = oldY;
    }
    Tank_Print(user, Field);

    return true;
}

bool tanks_g::Shooting(Player* user, int** Field, Bullet** bullets) {
    if (user == nullptr || Field == nullptr || bullets == nullptr || user->x_position < 0) return false;

    int index = 0;
    while (index < BULLET_COUNT && bullets[index] != nullptr && bullets[index]->Vector != 0) {
        index++;
    }
    if (index >= BULLET_COUNT || bullets[index] == nullptr) {
        return false;
    }

    int bulletX = user->x_position;
    int bulletY = user->y_position;

    if (user->angle == 1 || user->angle == -1) {
        bulletY += 2 * user->angle;
    } else if (user->angle == 2 || user->angle == -2) {
        bulletX += (user->angle == 2) ? 2 : -2;
    } else {
        return false;
    }

    if (IsWallCell(bulletX, bulletY)) {
        return false;
    }

    bullets[index]->owner = user->owner;
    bullets[index]->Vector = user->angle;
    bullets[index]->x_position = bulletX;
    bullets[index]->y_position = bulletY;
    Field[bulletY][bulletX]++;

    return true;
}

bool tanks_g::Bullet_Update(Bullet** bullets, int** Field) {
    if (bullets == nullptr || Field == nullptr) return false;

    for (int i = 0; i < BULLET_COUNT; i++) {
        if (bullets[i] == nullptr || bullets[i]->Vector == 0) continue;

        if (InBounds(bullets[i]->x_position, bullets[i]->y_position) && Field[bullets[i]->y_position][bullets[i]->x_position] > 0) {
            Field[bullets[i]->y_position][bullets[i]->x_position]--;
        }

        if (bullets[i]->Vector == 1 || bullets[i]->Vector == -1) {
            bullets[i]->y_position += bullets[i]->Vector;
        } else if (bullets[i]->Vector == 2 || bullets[i]->Vector == -2) {
            bullets[i]->x_position += (bullets[i]->Vector == 2) ? 1 : -1;
        } else {
            ResetBullet(bullets[i]);
            continue;
        }

        if (IsWallCell(bullets[i]->x_position, bullets[i]->y_position)) {
            ResetBullet(bullets[i]);
        } else {
            Field[bullets[i]->y_position][bullets[i]->x_position]++;
        }
    }

    return true;
}

void tanks_g::Bot_Init(Player** bots, int Number) {
    if (bots == nullptr) return;

    for (int i = 0; i < Number && i < BOT_COUNT; i++) {
        if (bots[i] == nullptr) continue;

        bots[i]->angle = 1;
        bots[i]->width = 3;
        bots[i]->owner = 2;

        if (i == 0) {
            bots[i]->y_position = Clamp(3, 2, HEIGHT - 3);
            bots[i]->x_position = Clamp(3, 2, WIDTH - 3);
        } else if (i == 1) {
            bots[i]->y_position = Clamp(3, 2, HEIGHT - 3);
            bots[i]->x_position = Clamp(WIDTH - 4, 2, WIDTH - 3);
        } else if (i == 2) {
            bots[i]->y_position = Clamp(HEIGHT - 4, 2, HEIGHT - 3);
            bots[i]->x_position = Clamp(5, 2, WIDTH - 3);
        } else if (i == 3) {
            bots[i]->y_position = Clamp(HEIGHT - 4, 2, HEIGHT - 3);
            bots[i]->x_position = Clamp(WIDTH - 6, 2, WIDTH - 3);
        } else {
            bots[i]->y_position = Clamp(2 + i, 2, HEIGHT - 3);
            bots[i]->x_position = Clamp(2 + i, 2, WIDTH - 3);
        }
    }
}

void tanks_g::Bot_Update(Player* bots[], int** Field, Bullet* bullets[]) {
    if (bots == nullptr || Field == nullptr || bullets == nullptr) return;

    for (int i = 0; i < BOT_COUNT; i++) {
        if (bots[i] == nullptr || bots[i]->x_position < 0) continue;

        int r = random(100);
        int vector = 0;

        if (r >= 80) {
            if (r % 5 == 0) Shooting(bots[i], Field, bullets);
            else if (r % 5 == 1) vector = 1;
            else if (r % 5 == 2) vector = 2;
            else if (r % 5 == 3) vector = -1;
            else if (r % 5 == 4) vector = -2;
        }

        if (vector != 0) {
            Body_Update(vector, bots[i], Field);
        }
    }
}

int tanks_g::Distance(Player* a, Player* b) {
    if (a == nullptr || b == nullptr || a->x_position < 0 || b->x_position < 0) return 1;

    int dx = abs(a->x_position - b->x_position);
    int dy = abs(a->y_position - b->y_position);
    return (dx >= 3 || dy >= 3) ? 1 : 0;
}

int tanks_g::Distance_B(Player* p, Bullet* b) {
    if (p == nullptr || b == nullptr || p->x_position < 0 || b->Vector == 0 || b->x_position < 0) return 1;

    int dx = abs(p->x_position - b->x_position);
    int dy = abs(p->y_position - b->y_position);
    return (dx >= 2 || dy >= 2) ? 1 : 0;
}

int tanks_g::Kill(Player* user, Player** bots, int** Field, Bullet** bullets) {
    if (user == nullptr || bots == nullptr || bullets == nullptr) return -100;

    for (int i = 0; i < BOT_COUNT; i++) {
        if (bots[i] != nullptr && bots[i]->x_position >= 0 && Distance(user, bots[i]) == 0) {
            return -1;
        }
    }

    for (int i = 0; i < BULLET_COUNT; i++) {
        if (bullets[i] == nullptr || bullets[i]->Vector == 0) continue;

        if (bullets[i]->owner != user->owner && Distance_B(user, bullets[i]) == 0) {
            return -1;
        }
    }

    for (int i = 0; i < BULLET_COUNT; i++) {
        if (bullets[i] == nullptr || bullets[i]->Vector == 0 || bullets[i]->owner != user->owner) continue;

        for (int j = 0; j < BOT_COUNT; j++) {
            if (bots[j] != nullptr && bots[j]->x_position >= 0 && Distance_B(bots[j], bullets[i]) == 0) {
                ResetBullet(bullets[i], Field);
                return j;
            }
        }
    }

    return -100;
}

void tanks_g::Real_Debugging(Player* bots[], Bullet* bullets[], Player* user, int** Field) {
    if (Field == nullptr) return;

    ResetField(Field);

    Tank_Print(user, Field);

    if (bots != nullptr) {
        for (int i = 0; i < BOT_COUNT; i++) {
            if (bots[i] != nullptr && bots[i]->x_position >= 0) {
                Tank_Print(bots[i], Field);
            }
        }
    }

    if (bullets != nullptr) {
        for (int i = 0; i < BULLET_COUNT; i++) {
            if (bullets[i] != nullptr && bullets[i]->Vector != 0 && !IsWallCell(bullets[i]->x_position, bullets[i]->y_position)) {
                Field[bullets[i]->y_position][bullets[i]->x_position]++;
            }
        }
    }
}

int** tanks_g::get_field() {
    return Field_Matrix;
}

void tanks_g::zeroing(){
    BTN_UP=0;
    BTN_DOWN=0;
    BTN_RIGHT=0;
    BTN_LEFT=0;
    BTN_FIRE=0;
}

// ==================== Управление кнопками ====================
int tanks_g::GetButtonCommand() {
    if (BTN_UP == 1) {
        return -1;
    }
    if (BTN_DOWN == 1) {
        return 1;
    }
    if (BTN_RIGHT == 1) {
        return 2;
    }
    if (BTN_LEFT == 1) {
        return -2;
    }
    if (BTN_FIRE == 1) {
        return 10;
    }
    return 0;
}

bool tanks_g::start_game() {
    ResetResources();

    Field_Matrix = Field_Init();
    Body_Matrix = Body_Init(&User);
    User.body = Body_Matrix;
    User.owner = 1;

    for (int i = 0; i < BULLET_COUNT; i++) {
        Bullets[i] = new Bullet;
        ResetBullet(Bullets[i]);
    }

    for (int i = 0; i < BOT_COUNT; i++) {
        Bots[i] = new Player;
        Bots[i]->body = Body_Init(Bots[i]);
        Bots[i]->x_position = -1;
        Bots[i]->y_position = -1;
        Bots[i]->owner = 2;
    }
    Bot_Init(Bots, BOT_COUNT);

    score = 0;
    record = 0;
    killCode = 0;
    timer = PAUSE;

    Real_Debugging(Bots, Bullets, &User, Field_Matrix);
    return true;
}

void tanks_g::set_buttons(const std::vector<int>& buttons) {
    // if (timer-- > 0) {
    //     return;
    // }
    // timer = PAUSE;
    if (buttons.size() < 5) {
        BTN_UP = 0;
        BTN_DOWN = 0;
        BTN_LEFT = 0;
        BTN_RIGHT = 0;
        BTN_FIRE = 0;
        return;
    }
    if (buttons[0]==1){
        BTN_UP = 1;
        BTN_DOWN = 0;
    }
    else if (buttons[0]==-1){
        BTN_UP = 0;
        BTN_DOWN = 1;
    }
    if (buttons[1]==1){
        BTN_LEFT = 1;
        BTN_RIGHT = 0;
    }
    else if (buttons[1]==-1){
        BTN_LEFT = 0;
        BTN_RIGHT = 1;
    }
    BTN_FIRE = buttons[2];
    if (buttons[TERMINATE_BUTTON]==1){
        killCode = -1;
    }
}

bool tanks_g::continue_game() {
    if (killCode == -1) {
        return false;
    }

    if (timer-- > 0) {
        return true;
    }
    timer = PAUSE;

    int cmd = GetButtonCommand();
    
    zeroing();

    if (cmd != 0) {
        if (abs(cmd) <= 2 && cmd != 10) {
            Body_Update(cmd, &User, Field_Matrix);
        } else if (cmd == 10) {
            Shooting(&User, Field_Matrix, Bullets);
        }
    }

    Bullet_Update(Bullets, Field_Matrix);

    if (!ApplyKillCode(Kill(&User, Bots, Field_Matrix, Bullets), Field_Matrix)) {
        return false;
    }

    Bot_Update(Bots, Field_Matrix, Bullets);

    if (!ApplyKillCode(Kill(&User, Bots, Field_Matrix, Bullets), Field_Matrix)) {
        return false;
    }

    Real_Debugging(Bots, Bullets, &User, Field_Matrix);

    if (score > record) {
        record = score;
    }

    int aliveBots = 0;
    for (int i = 0; i < BOT_COUNT; i++) {
        if (Bots[i] != nullptr && Bots[i]->x_position >= 0) {
            aliveBots++;
        }
    }

    if (aliveBots == 0) {
        Bot_Init(Bots, BOT_COUNT);
        for (int i = 0; i < BOT_COUNT; i++) {
            int dx = abs(User.x_position - Bots[i]->x_position);
            int dy = abs(User.y_position - Bots[i]->y_position);
            if (dx <= 2 && dy <= 2) {
                Bots[i]->x_position = -2;
                Bots[i]->y_position = -2;
            }
        }
        Real_Debugging(Bots, Bullets, &User, Field_Matrix);
    }

    return true;
}

bool tanks_g::game_continue() {
    return killCode != -1;
}
