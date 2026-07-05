#ifndef REWARD_H
#define REWARD_H

#include "raylib.h"
#include "../common/states.h"
#include "../common/audio_manager.h"
#include "shoots.h"

#define MAX_REWARDS 150

typedef enum RewardType {
    REWARD_SCORE,       
    REWARD_HEAL,        
    REWARD_SPEED,      
    REWARD_DAMAGE,      
    REWARD_COOLDOWN   
} RewardType;

typedef struct {
    int score;
    int hp;
    float damageBonus;
    float speedBonus;
    float cooldownBonus;
} RewardGain;


typedef struct {
    Vector2 position;
    RewardType type;
    int rarity;
    bool active;
} RewardItem;

void InitRewards(void);
void EnemyDeath(Vector2 position, int rarity);
RewardGain UpdateAndCheckRewardCollisions(Vector2 playerPos);
void DrawRewards(void);

#endif