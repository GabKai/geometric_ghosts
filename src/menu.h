#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include "common/states.h"
#include "common/audio_manager.h"

void InitMenu(GameData *gData, ScoreEntry *score);
void ResetMenu(void);
void UpdateMenu(void);
void DrawMenu(void);
void UpdateGameOver(void);
void DrawGameOver(void);
void UnloadMenu(void);

#endif