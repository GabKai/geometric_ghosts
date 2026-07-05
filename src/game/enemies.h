#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"
#include "shoots.h"

#define MAX_ENEMY_TEMPLATES 16
#define MAX_ACTIVE_ENEMIES 100
#define INITAL_SPAWN_INTERVAL 5.0f
#define SPAWN_INTERVAL_DECREASE 0.25f
#define MIN_SPAWN_INTERVAL 1.5f

typedef struct {
    char name[32];
    int rarity;
    int maxHp;
    int size;
    int frames;
    float speed;
    int damage;
    float shootCooldown;
    float minDist;

    bool textureLoaded;
    Texture2D texture;  
    
    bool hasShoot;    
    char shootTemplateName[32];
    ShooterInfo shootShooterInfo; 
    
    bool hasDeath;
    char deathTemplateName[32];
    ShooterInfo deathShooterInfo; 
} EnemyTemplate;

typedef struct {
    bool active;
    EnemyTemplate config;
    Vector2 position;
    int hp;
    int frame;
    float frameTimer;
    float cooldownTimer;
    float stunTimer;
} Enemy;

void LoadEnemyTemplates(void);
void SpawnEnemy(Vector2 playerPos);
void UpdateEnemies(Vector2 playerPos);
void DrawEnemies(void);
int GetActiveEnemiesCount(void);
Enemy* GetEnemyInstance(int index);

#endif