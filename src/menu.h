#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "common/states.h"
#include "common/audio_manager.h"

typedef struct {
    char nick[11];
    int score;
} ScoreRecord;

void InitMenu(void);
void UpdateMenu(GameState *currentState, char *playerNick);
void DrawMenu(const char *playerNick);
void UnloadMenu(void);

#endif