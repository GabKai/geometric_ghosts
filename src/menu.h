#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "common/states.h"
#include "common/audio_manager.h"

typedef struct {
    char nick[16];
    int score;
} ScoreRecord;

void InitMenu(GameData *gData, ScoreRecord *score);
void UpdateMenu(void);
void DrawMenu(void);
void UpdateGameOver(void);
void DrawGameOver(void);
void UnloadMenu(void);

#endif