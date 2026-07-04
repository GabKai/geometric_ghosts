#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "common/states.h"
#include "common/audio_manager.h"

typedef struct {
    char nick[11];
    int score;
} ScoreRecord;

void InitMenu(GameData *gData);
void UpdateMenu(void);
void DrawMenu(void);
void UpdateGameOver(void);
void DrawGameOver(void);
void UnloadMenu(void);

#endif