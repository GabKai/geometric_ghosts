#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "menu.h"
#include "common/states.h"
#include "game/shoots.h"
#include "game/enemies.h"
#include "game/reward.h"

typedef struct {
    Texture2D texture;
    Vector2 position;
    int size;
    int maxHp;
    int hp;
    float speed;
    ShooterInfo bonus;
    float invulnTimer;       
    float blinkTimer;        
    bool isVisible;
} Player;

void InitGame(GameData *gData);
void UpdateGame(void);
void DrawGame(void);
void DrawUI(void);
void UnloadGame(void);
void CheckCollisions(void);
void CheckPlayerEnemiesCollisions(void);
void CheckProjectilesCollisions(void);
void PlayerDamaged(int damage);

#endif