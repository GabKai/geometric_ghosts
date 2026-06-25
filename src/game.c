#include "game.h"
#include <string.h>
#include <math.h>

static Player player;
static Camera2D camera;
static bool textureLoaded = false;

void InitGame(const char *playerNick) {
    player.position = (Vector2){ 0.0f, 0.0f }; 
    player.speed = 300.0f; 
    strncpy(player.nick, playerNick, 10);
    player.nick[10] = '\0';

    if (FileExists("assets/player/sprite_base.png")) {
        player.texture = LoadTexture("assets/player/sprite_base.png");
        textureLoaded = true;
    }

    camera.target = player.position;
    camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void UpdateGame(GameState *currentState) {
    float deltaTime = GetFrameTime();

    Vector2 movement = { 0.0f, 0.0f };

    if (IsKeyDown(KEY_W)) movement.y -= 1.0f;
    if (IsKeyDown(KEY_S)) movement.y += 1.0f;
    if (IsKeyDown(KEY_A)) movement.x -= 1.0f;
    if (IsKeyDown(KEY_D)) movement.x += 1.0f;

    if (movement.x != 0.0f || movement.y != 0.0f) {
        float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
        
        player.position.x += (movement.x / length) * player.speed * deltaTime;
        player.position.y += (movement.y / length) * player.speed * deltaTime;
    }

    camera.target = player.position;

    if (IsKeyPressed(KEY_ESCAPE)) {
        *currentState = STATE_MENU;
    }
}

void DrawGame(void) {
    ClearBackground(BLACK);

    BeginMode2D(camera);

        int gridSize = 160; 
        
        float startX = player.position.x - GetScreenWidth() / 2.0f - gridSize;
        float endX = player.position.x + GetScreenWidth() / 2.0f + gridSize;
        float startY = player.position.y - GetScreenHeight() / 2.0f - gridSize;
        float endY = player.position.y + GetScreenHeight() / 2.0f + gridSize;

        startX = ((int)startX / gridSize) * gridSize;
        startY = ((int)startY / gridSize) * gridSize;

        for (float x = startX; x <= endX; x += gridSize) {
            DrawLineV((Vector2){ x, startY }, (Vector2){ x, endY }, DARKGRAY);
        }
        for (float y = startY; y <= endY; y += gridSize) {
            DrawLineV((Vector2){ startX, y }, (Vector2){ endX, y }, DARKGRAY);
        }

        if (textureLoaded) {
            Vector2 positionOffset = { 
                player.position.x - player.texture.width / 2.0f, 
                player.position.y - player.texture.height / 2.0f 
            };
            DrawTextureV(player.texture, positionOffset, WHITE);
        } else {
            DrawCircleV(player.position, 20, PURPLE);
            DrawCircleV((Vector2){player.position.x - 7, player.position.y - 5}, 4, WHITE); // Olho E
            DrawCircleV((Vector2){player.position.x + 7, player.position.y - 5}, 4, WHITE); // Olho D
        }

    EndMode2D();

    DrawText("Pressione ESC para voltar ao Menu", 10, 10, 20, RAYWHITE);
}

void UnloadGame(void) {
    if (textureLoaded) {
        UnloadTexture(player.texture);
        textureLoaded = false;
    }
}