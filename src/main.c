#include "raylib.h"
#include <stdio.h>
#include "menu.h"
#include "game.h"
#include "common/states.h"

static GameData gameData;
static ScoreEntry scoreboard[6];

void LoadScoreboard(void);
void SaveToScoreboard(void);

int main(void)
{
    const int screenWidth = 1500;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Geometric Ghosts");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    gameData.currentState = STATE_MENU;
    gameData.playerNick[10] = '\0';
    gameData.score = 0;
    bool gameInitialized = false;

    LoadScoreboard();
    InitAudioManager();
    InitMenu(&gameData, scoreboard);

    while (!WindowShouldClose())
    {
        switch (gameData.currentState)
        {
        case STATE_MENU:
            UpdateMenu();
            gameInitialized = false;
            break;
        case STATE_GAME:
            if (!gameInitialized)
            {
                gameData.score = 0;
                InitGame(&gameData);
                gameInitialized = true;
            }
            UpdateGame();
            break;
        case STATE_GAMEUP:
            break;
        case STATE_GAMEOVER:
            if (gameInitialized){   
                SaveToScoreboard();
                ResetMenu();
                gameInitialized = false;
                InitMenu(&gameData, scoreboard);
            }
            UpdateGameOver();
            break;
        default:
            break;
        }

        BeginDrawing();

        switch (gameData.currentState)
        {
        case STATE_MENU:
            DrawMenu();
            break;
        case STATE_GAME:
            if (gameInitialized)
            {
                DrawGame();
                DrawUI();
            }
            break;
        case STATE_GAMEUP:
            DrawGame();
            DrawUI();
            DrawLevelUp();
            break;
        case STATE_GAMEOVER:
            DrawGame();
            DrawGameOver();
            break;
        default:
            break;
        }

        EndDrawing();
    }

    UnloadAudioManager();
    UnloadMenu();
    if (gameInitialized){     
        UnloadGame();
    }

    CloseWindow();

    return 0;
}

void LoadScoreboard(void){
    for (int i = 0; i < 6; i++) {
        scoreboard[i].score = 0;
        scoreboard[i].nick[0] = '\0';
    }

    FILE *file = fopen("info/scoreboard.txt", "r");
    if (file == NULL) {
        TraceLog(LOG_INFO, "SCOREBOARD: Arquivo nao encontrado. Criando uma lista limpa.");
        return;
    }

    int count = 0;
    while (count < 5 && fscanf(file, "%15s %d", scoreboard[count].nick, &scoreboard[count].score) == 2) {
        count++;
    }

    fclose(file);
    TraceLog(LOG_INFO, "SCOREBOARD: %d recordes carregados com sucesso!", count);
}

void SaveToScoreboard(void) {
    int count = 0;
    while (count < 5 && scoreboard[count].score > 0) {
        count++;
    }

    strncpy(scoreboard[count].nick, gameData.playerNick, sizeof(scoreboard[count].nick) - 1);
    scoreboard[count].nick[sizeof(scoreboard[count].nick) - 1] = '\0';
    scoreboard[count].score = gameData.score;
    count++; 

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scoreboard[j].score < scoreboard[j + 1].score) {
                ScoreEntry temp = scoreboard[j];
                scoreboard[j] = scoreboard[j + 1];
                scoreboard[j + 1] = temp;
            }
        }
    }

    if (count > 5) {
        scoreboard[5].score = 0;
        scoreboard[5].nick[0] = '\0';
    }

    FILE *file = fopen("info/scoreboard.txt", "w");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Nao foi possivel abrir o txt para escrita!");
        return;
    }

    for (int i = 0; i < 5; i++) {
        if (i >= count) { break; }

        fprintf(file, "%s %d\n", scoreboard[i].nick, scoreboard[i].score);
    }
    
    fclose(file);
    TraceLog(LOG_INFO, "SCOREBOARD: Salvo com sucesso e atualizado na memoria!");
}