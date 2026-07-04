#include "raylib.h"
#include "menu.h"
#include "game.h"
#include "common/states.h"

int main(void)
{
    const int screenWidth = 1500;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Geometric Ghosts");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    GameData gameData;
    gameData.currentState = STATE_MENU;
    gameData.playerNick[10] = '\0';
    gameData.score = 0;
    bool gameInitialized = false;

    InitAudioManager();
    InitMenu(&gameData);

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
                InitGame(&gameData);
                gameInitialized = true;
            }
            UpdateGame();
            break;
        case STATE_GAMEOVER:
            UpdateGameOver();
            gameInitialized = false;
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
            DrawGame();
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