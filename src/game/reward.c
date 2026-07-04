#include "reward.h"
#include <math.h>

static const float SCORE_SIZE = 7.0f;

static RewardItem rewards[MAX_REWARDS];

void InitRewards(void) {
    for (int i = 0; i < MAX_REWARDS; i++) {
        rewards[i].active = false;
    }
}

void EnemyDeath(Vector2 position, int rarity) {
    int index = -1;
    for (int i = 0; i < MAX_REWARDS; i++) {
        if (!rewards[i].active) {
            index = i;
            break;
        }
    }

    if (index == -1) return;

    rewards[index].position = position;
    rewards[index].type = REWARD_SCORE;
    rewards[index].rarity = rarity;
    rewards[index].active = true;
}

void UpdateAndCheckRewardCollisions(Vector2 playerPos, int *playerHp, int *currentScore) {
    float playerRadius = 16.0f;

    float rewardRadius = SCORE_SIZE / 2.0f;

    for (int i = 0; i < MAX_REWARDS; i++) {
        if (!rewards[i].active) continue;

        if (CheckCollisionCircles(playerPos, playerRadius, rewards[i].position, rewardRadius)) {
            
            switch(rewards[i].type) {
                case REWARD_SCORE:
                int scoreGain = 50*(rewards[i].rarity + 1);
                int hpHeal = rewards[i].rarity + 2;

                *currentScore += scoreGain;
                *playerHp += hpHeal;

                if (*playerHp > 100) {
                    *playerHp = 100;
                }

                rewards[i].active = false;
                PlaySFX("game/score.wav");
                break;
            }
        }
    }
}

void DrawRewards(void) {
    for (int i = 0; i < MAX_REWARDS; i++) {
        if (!rewards[i].active) continue;

        switch(rewards[i].type) {
            case REWARD_SCORE:
            DrawCircleV(rewards[i].position, SCORE_SIZE, RAYWHITE); 
            break;
        }
    }
}