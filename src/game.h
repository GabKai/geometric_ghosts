#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "menu.h"
#include "common/states.h"
#include "game/shoots.h"
#include "game/enemies.h"
#include "game/reward.h"

#define MIN_COOLDOWN 0.2f

typedef struct {
    Texture2D texture;
    Vector2 position;
    int size;
    int maxHp;
    int hp;
    float speed;
    float damageModifier;
    float cooldownModifier;
    ShootTemplate shootTemplate;
    ShooterInfo bonus;
    float invulnTimer;       
    float blinkTimer;        
    bool isVisible;
} Player;

typedef struct {
    ShootTemplate *options[2];
    float clickTimer;           
    int selectedOption;         
} LevelUpScreenState;

void InitGame(GameData *gData);
void UpdateGame(void);
void DrawGame(void);
void DrawUI(void);
void DrawLevelUp(void);
void UnloadGame(void);
void CheckCollisions(void);
void CheckPlayerEnemiesCollisions(void);
void CheckProjectilesCollisions(void);
void PlayerDamaged(int damage);
void TriggerLevelUp(void);

#endif