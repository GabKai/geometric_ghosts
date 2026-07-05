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
    rewards[index].rarity = rarity;
    rewards[index].active = true;

    int rng = GetRandomValue(0, 100);
    if (rng < 40) {
        rewards[index].type = REWARD_SCORE;  
    } else if (rng < 55) {
        rewards[index].type = REWARD_HEAL;     
    } else if (rng < 70) {
        rewards[index].type = REWARD_COOLDOWN; 
    } else if (rng < 85) {
        rewards[index].type = REWARD_DAMAGE;  
    } else {
        rewards[index].type = REWARD_SPEED;   
    }
}

RewardGain UpdateAndCheckRewardCollisions(Vector2 playerPos) {
    float playerRadius = 16.0f;

    float rewardRadius = SCORE_SIZE;
    RewardGain totalGain = { 0, 0, 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < MAX_REWARDS; i++) {
        if (!rewards[i].active) continue;

        if (CheckCollisionCircles(playerPos, playerRadius, rewards[i].position, rewardRadius)) {
            
            int baseScore = 50 * (rewards[i].rarity + 1);
            totalGain.score += baseScore;

            switch(rewards[i].type) {
                case REWARD_SCORE:
                    break;
                case REWARD_HEAL:
                    totalGain.hp += 5 * (rewards[i].rarity + 1);
                    break;
                case REWARD_SPEED:
                    totalGain.speedBonus += 15.0f * (rewards[i].rarity + 1);
                    break;
                case REWARD_DAMAGE:
                    totalGain.damageBonus += 0.075f * (rewards[i].rarity + 1);
                    break;
                case REWARD_COOLDOWN:
                    totalGain.cooldownBonus += 0.04f * (rewards[i].rarity + 1);
                    break;
            }
            
            rewards[i].active = false;
            PlaySFX("game/score.wav");
        }
    }
    return totalGain;
}

void DrawRewards(void) {
    for (int i = 0; i < MAX_REWARDS; i++) {
        if (!rewards[i].active) continue;

        Color rewardColor = RAYWHITE;

        switch(rewards[i].type) {
            case REWARD_SCORE:    rewardColor = RAYWHITE; break; 
            case REWARD_HEAL:     rewardColor = GREEN;    break; 
            case REWARD_SPEED:    rewardColor = SKYBLUE;  break; 
            case REWARD_DAMAGE:   rewardColor = YELLOW;   break; 
            case REWARD_COOLDOWN: rewardColor = PINK;     break; 
        }

        DrawCircleV(rewards[i].position, SCORE_SIZE, rewardColor);
    }
}