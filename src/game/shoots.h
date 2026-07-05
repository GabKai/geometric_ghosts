#ifndef SHOOTS_H
#define SHOOTS_H

#include "raylib.h"

#define MAX_SHOOT_TEMPLATES 16
#define MAX_PROJECTILES_PER_GROUP 32
#define MAX_ACTIVE_GROUPS 64
#define PROJECTILE_SIZE_BASE 4.0f

typedef enum {
    MOVE_LINEAR,
    MOVE_CIRCLE,
    MOVE_SINE,
    MOVE_SPLASH
} MoveType;

typedef struct {
    char name[32];
    MoveType moveType;
    float cooldown;
    int count;
    int damage;
    float duration;
    float refSpeed;
    char sound[32];
    float aux1;
    float aux2;
    float aux3;
} ShootTemplate;

typedef struct {
    Vector2 localPosition; 
    float offset;     
    bool active;
} Projectile;

typedef struct {
    bool active;
    bool isAlly;         
    ShootTemplate config;
    int count;
    Vector2 refPosition;   
    Vector2 direction;     
    float currentTime;
    int radius;    
    int damage;
    float refSpeed;
    float aux1;
    float aux2;
    float aux3;
    
    Projectile projectiles[MAX_PROJECTILES_PER_GROUP];
} ShootGroup;

typedef struct {
    int bonusDamage;
    float bonusSpeed;
    float aux1Mod;
    float aux2Mod;
    float aux3Mod;
} ShooterInfo;

void LoadShootTemplates(void);
void SpawnShoot(const char *name, Vector2 origin, Vector2 target, ShooterInfo shooter, bool isAlly);
void UpdateShoots(void);
void DrawShoots(void);
int GetActiveShootGroupsCount(void);
ShootGroup* GetShootGroupInstance(int index);
ShootTemplate* GetShootTemplate(const char *name);
ShootTemplate* GetRandomShootTemplate(void);

#endif