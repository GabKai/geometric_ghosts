#ifndef STATES_H
#define STATES_H

typedef enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_GAMEOVER,
    STATE_OPTIONS,
} GameState;

typedef struct {
    GameState currentState;
    char playerNick[11];
    int score;
} GameData;

#endif