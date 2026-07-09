#ifndef STATES_H
#define STATES_H

typedef enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_GAMEUP,
    STATE_GAMEOVER,
    STATE_OPTIONS,
} GameState;

typedef struct {
    GameState currentState;
    char playerNick[11];
    int score;
} GameData;

typedef struct {
    char nick[16];
    int score;
} ScoreEntry;

#endif