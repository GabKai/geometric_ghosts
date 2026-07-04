#include "menu.h"
#include <string.h>
#include <ctype.h>

static *ScoreRecord scoreboard;
static int scoreCount = 0;
static int letterCount = 0;

static GameData *gameData;

void InitMenu(GameData *gData, ScoreRecord *score) {
    gameData = gData;
    scoreboard = score;

    for (int i = 0; i < 5; i++) {
        if (scoreboard[i].score > 0) {
            scoreCount++;
        }
    }
}

void UpdateMenu(void) {
    int key = GetCharPressed();
    
    while (key > 0) {
        if (((key >= 'a' && key <= 'z') || 
             (key >= 'A' && key <= 'Z') || 
             (key >= '0' && key <= '9') || 
             key == '_') && (letterCount < 10)) 
        {
            gameData->playerNick[letterCount] = (char)key;
            letterCount++;
            gameData->playerNick[letterCount] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        letterCount--;
        if (letterCount < 0) letterCount = 0;
        gameData->playerNick[letterCount] = '\0';
    }

    bool isNickValid = (letterCount >= 3 && letterCount <= 10);
    
    Rectangle playButton = { GetScreenWidth()/2 - 100, 450, 200, 50 };
    Vector2 mousePos = GetMousePosition();

    if (isNickValid && CheckCollisionPointRec(mousePos, playButton)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            gameData->currentState = STATE_GAME; 
            PlaySFX("ui/clickSelect.wav");
        }
    }
}

void DrawMenu(void) {
    ClearBackground(BLACK);

    DrawText("GEOMETRIC GHOSTS", GetScreenWidth()/2 - MeasureText("GEOMETRIC GHOSTS", 40)/2, 50, 40, PURPLE);

    DrawText("DIGITE SEU NICK (3-10 carac.):", GetScreenWidth()/2 - 150, 150, 18, LIGHTGRAY);
    
    DrawRectangleLines(GetScreenWidth()/2 - 150, 180, 300, 40, DARKGRAY);
    if (letterCount > 0) {
        DrawText(gameData->playerNick, GetScreenWidth()/2 - 140, 190, 20, RAYWHITE);
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

void UpdateGameOver(void) {
    Rectangle retryButton = { GetScreenWidth()/2 - 120, 240, 240, 50 };
    Rectangle menuButton = { GetScreenWidth()/2 - 120, 310, 240, 50 };
    
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, retryButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        gameData->currentState = STATE_GAME; 
        PlaySFX("ui/clickSelect.wav");
    }

    if (CheckCollisionPointRec(mousePos, menuButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        gameData->currentState = STATE_MENU;
        PlaySFX("ui/clickSelect.wav");
    }
}

void DrawGameOver(void) {
    const char *title = "GAME OVER";
    DrawText(title, GetScreenWidth()/2 - MeasureText(title, 40)/2, 50, 40, RED);

    char scoreBuffer[50];
    sprintf(scoreBuffer, "PONTUACAO FINAL: %d", gameData->score);
    DrawText(scoreBuffer, GetScreenWidth()/2 - MeasureText(scoreBuffer, 22)/2, 140, 22, GOLD);

    Rectangle retryButton = { GetScreenWidth()/2 - 120, 240, 240, 50 };
    Rectangle menuButton = { GetScreenWidth()/2 - 120, 310, 240, 50 };
    
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, retryButton)) {
        DrawRectangleRec(retryButton, DARKPURPLE);
    } else {
        DrawRectangleRec(retryButton, PURPLE);
    }
    const char *retryText = "JOGAR DE NOVO";
    DrawText(retryText, retryButton.x + (retryButton.width/2 - MeasureText(retryText, 18)/2), retryButton.y + 16, 18, WHITE);

    if (CheckCollisionPointRec(mousePos, menuButton)) {
        DrawRectangleRec(menuButton, DARKGRAY);
    } else {
        DrawRectangleRec(menuButton, GRAY);
    }
    const char *menuText = "VOLTAR AO MENU";
    DrawText(menuText, menuButton.x + (menuButton.width/2 - MeasureText(menuText, 18)/2), menuButton.y + 16, 18, WHITE);
}

void UnloadMenu(void) {
}