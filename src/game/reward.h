#ifndef REWARD_H
#define REWARD_H

#include "raylib.h"
#include "../common/states.h"
#include "../common/audio_manager.h"

#define MAX_REWARDS 150

typedef enum RewardType {
    REWARD_SCORE
} RewardType;

typedef struct {
    Vector2 position;
    RewardType type;
    int rarity;
    bool active;
} RewardItem;

void InitRewards(void);
void EnemyDeath(Vector2 position, int rarity);
void UpdateAndCheckRewardCollisions(Vector2 playerPos, int *playerHp, int *currentScore);
void DrawRewards(void);

#endif