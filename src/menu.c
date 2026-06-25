#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static ScoreRecord scores[5];
static int scoreCount = 0;
static float volume = 1.0f;
static int letterCount = 0;

void InitMenu(void) {
    FILE *fileOpts = fopen("info/options.txt", "r");
    if (fileOpts != NULL) {
        fscanf(fileOpts, "%f", &volume);
        fclose(fileOpts);
    }
        
    SetMasterVolume(volume);

    FILE *fileScore = fopen("info/scoreboard.txt", "r");
    scoreCount = 0;
    if (fileScore != NULL) {
        while (scoreCount < 5 && fscanf(fileScore, "%s %d", scores[scoreCount].nick, &scores[scoreCount].score) == 2) {
            scoreCount++;
        }
        fclose(fileScore);
    }
}

void UpdateMenu(GameState *currentState, char *playerNick) {
    int key = GetCharPressed();
    
    while (key > 0) {
        if (((key >= 'a' && key <= 'z') || 
             (key >= 'A' && key <= 'Z') || 
             (key >= '0' && key <= '9') || 
             key == '_') && (letterCount < 10)) 
        {
            playerNick[letterCount] = (char)key;
            letterCount++;
            playerNick[letterCount] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        letterCount--;
        if (letterCount < 0) letterCount = 0;
        playerNick[letterCount] = '\0';
    }

    bool isNickValid = (letterCount >= 3 && letterCount <= 10);
    
    Rectangle playButton = { GetScreenWidth()/2 - 100, 450, 200, 50 };
    Vector2 mousePos = GetMousePosition();

    if (isNickValid && CheckCollisionPointRec(mousePos, playButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *currentState = STATE_GAME; 
        }
    }
}

void DrawMenu(const char *playerNick) {
    ClearBackground(BLACK);

    DrawText("GEOMETRIC GHOSTS", GetScreenWidth()/2 - MeasureText("GEOMETRIC GHOSTS", 40)/2, 50, 40, PURPLE);

    DrawText("DIGITE SEU NICK (3-10 carac.):", GetScreenWidth()/2 - 150, 150, 18, LIGHTGRAY);
    
    DrawRectangleLines(GetScreenWidth()/2 - 150, 180, 300, 40, DARKGRAY);
    if (letterCount > 0) {
        DrawText(playerNick, GetScreenWidth()/2 - 140, 190, 20, RAYWHITE);
    }

    DrawText("TOP 5 SCOREBOARD", GetScreenWidth()/2 - 100, 260, 20, GOLD);
    int startY = 300;
    for (int i = 0; i < scoreCount; i++) {
        char buffer[50];
        sprintf(buffer, "%d. %-12s %d", i + 1, scores[i].nick, scores[i].score);
        DrawText(buffer, GetScreenWidth()/2 - 100, startY + (i * 25), 18, GRAY);
    }
    if (scoreCount == 0) {
        DrawText("Nenhum recorde registrado.", GetScreenWidth()/2 - 110, startY, 16, DARKGRAY);
    }

    bool isNickValid = (letterCount >= 3 && letterCount <= 10);
    Rectangle playButton = { GetScreenWidth()/2 - 100, 450, 200, 50 };
    Vector2 mousePos = GetMousePosition();

    if (!isNickValid) {
        DrawRectangleRec(playButton, DARKGRAY);
        DrawText("PLAY", playButton.x + 70, playButton.y + 15, 20, GRAY);
    } else {
        if (CheckCollisionPointRec(mousePos, playButton)) {
            DrawRectangleRec(playButton, DARKPURPLE);
        } else {
            DrawRectangleRec(playButton, PURPLE);
        }
        DrawText("PLAY", playButton.x + 70, playButton.y + 15, 20, WHITE);
    }
}

void UnloadMenu(void) {
}