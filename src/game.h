#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "menu.h"
#include "common/states.h"
#include "game/shoots.h"
#include "game/enemies.h"

typedef struct {
    Vector2 position;
    float speed;
    Texture2D texture;
    ShooterInfo bonus;
    char nick[11];
} Player;

void InitGame(const char *playerNick);
void UpdateGame(GameState *currentState);
void DrawGame(void);
void UnloadGame(void);

#endif