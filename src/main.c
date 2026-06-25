#include "raylib.h"
#include "menu.h"
#include "game.h"
#include "states.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Geometric Ghosts");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    GameState currentState = STATE_MENU;
    char playerNick[11] = "\0";

    bool gameInitialized = false;

    InitMenu();

    while (!WindowShouldClose())
    {
        switch (currentState)
        {
        case STATE_MENU:
            UpdateMenu(&currentState, playerNick);
            gameInitialized = false;
            break;
        case STATE_GAME:
            if (!gameInitialized)
            {
                InitGame(playerNick);
                gameInitialized = true;
            }
            UpdateGame(&currentState);
            break;
        default:
            break;
        }

        BeginDrawing();

        switch (currentState)
        {
        case STATE_MENU:
            DrawMenu(playerNick);
            break;
        case STATE_GAME:
            DrawGame();
            break;
        default:
            break;
        }

        EndDrawing();
    }

    UnloadMenu();
    if (gameInitialized){        
        UnloadGame();
    }

    CloseWindow();

    return 0;
}