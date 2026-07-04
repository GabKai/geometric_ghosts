#include "game.h"
#include <string.h>
#include <math.h>

const static float INVUL_TIME = 3.0f;
const static float STUN_TIME = 1.0f;

static Player player;
static Camera2D camera;
static bool textureLoaded = false;

static GameData *gameData;

void InitGame(GameData *gData) {
    gameData = gData;

    player.hp = 1;
    player.maxHp = 1;
    player.size = 32;
    player.position = (Vector2){ 0.0f, 0.0f }; 
    player.isVisible = true;
    player.speed = 300.0f; 
    player.bonus = (ShooterInfo){ 0.0f, 0.0f, 0.0f, 0 };
    player.blinkTimer = 0.0f;
    player.invulnTimer = 0.0f;
    player.isVisible = true;

    if (FileExists("assets/sprites/player/sprite_base.png")) {
        player.texture = LoadTexture("assets/sprites/player/sprite_base.png");
        textureLoaded = true;
    }

    LoadShootTemplates();
    LoadEnemyTemplates();

    camera.target = player.position;
    camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void UpdateGame(void) {
    float deltaTime = GetFrameTime();

    CheckCollisions();

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

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        SpawnShoot("simple", player.position, mouseWorldPos, player.bonus, true);
    }

    UpdateEnemies(player.position);
    UpdateShoots();

    SpawnEnemy(player.position);

    if (IsKeyPressed(KEY_ESCAPE)) {
        gameData->currentState = STATE_MENU;
    }
}

void DrawGame(void) {
    float deltaTime = GetFrameTime();

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
        
        DrawEnemies();

        if (player.invulnTimer > 0.0f) {      
            player.blinkTimer += deltaTime;
            if (player.blinkTimer >= 0.1f) {
                player.isVisible = !player.isVisible;
                player.blinkTimer = 0.0f;
            }
        }

        if (player.isVisible && textureLoaded) {
            Vector2 positionOffset = { 
                player.position.x - player.texture.width / 2.0f, 
                player.position.y - player.texture.height / 2.0f 
            };
            DrawTextureV(player.texture, positionOffset, WHITE);
        }

        DrawShoots();

    EndMode2D();

    DrawText("Pressione ESC para voltar ao Menu", 10, 10, 20, RAYWHITE);
}

void UnloadGame(void) {
    if (textureLoaded) {
        UnloadTexture(player.texture);
        textureLoaded = false;
    }
}

void CheckCollisions(void){    
    CheckPlayerEnemiesCollisions();
    CheckProjectilesCollisions();
}

void CheckPlayerEnemiesCollisions(void) {
    float deltaTime = GetFrameTime();

    if (player.invulnTimer > 0.0f) {
        player.invulnTimer -= deltaTime;
        if (player.invulnTimer <= 0.0f) {
            player.isVisible = true;
        }
    }

    if (player.invulnTimer > 0.0f) return;

    int totalEnemies = GetActiveEnemiesCount();
    float playerRadius = player.size/2.0f;

    for (int i = 0; i < totalEnemies; i++) {
        Enemy *e = GetEnemyInstance(i);
        if (e == NULL) continue;

        float enemyRadius = e->config.size / 2.0f;

        if (CheckCollisionCircles(player.position, playerRadius, e->position, enemyRadius)) {
            PlayerDamaged(e->config.damage);

            e->stunTimer = STUN_TIME;

            TraceLog(LOG_INFO, "GAME: Player colidiu com %s! Tomou dano. Invulneravel por 5s.", e->config.name);
            break;
        }
    }
}

void CheckProjectilesCollisions(void){
    int totalGroups = GetActiveShootGroupsCount();
    int totalEnemies = GetActiveEnemiesCount();
    
    float playerRadius = player.size / 2.0f;
    float shootRadius = 3.0f;

    for (int g = 0; g < totalGroups; g++) {
        ShootGroup *group = GetShootGroupInstance(g);
        if (group == NULL) continue;

        for (int p = 0; p < group->config.count; p++) {
            if (!group->projectiles[p].active) continue;

            Vector2 projectilePos = group->projectiles[p].localPosition;

            if (group->isAlly) {
                for (int e = 0; e < totalEnemies; e++) {
                    Enemy *enemy = GetEnemyInstance(e);
                    if (enemy == NULL) continue;

                    float enemyRadius = enemy->config.size / 2.0f;

                    if (CheckCollisionCircles(projectilePos, shootRadius, enemy->position, enemyRadius)) {
                        enemy->hp -= group->config.damage;
                        enemy->stunTimer = STUN_TIME / 3.0f;

                        group->projectiles[p].active = false;

                        if (enemy->hp <= 0) {
                            enemy->active = false;
                            
                            SpawnShoot(enemy->config.deathTemplateName, enemy->position, player.position, enemy->config.deathShooterInfo, false);
                        }                        
                        break; 
                    }
                }
            } else {
                if (player.invulnTimer > 0.0f) continue;

                if (CheckCollisionCircles(projectilePos, shootRadius, player.position, playerRadius)) {

                    PlayerDamaged(group->config.damage);
                    group->projectiles[p].active = false;

                    TraceLog(LOG_INFO, "GAME: Player foi atingido por um projetil inimigo! HP atual: %d", player.hp);
                    break;
                }
            }
        }

        bool anyProjectileActive = false;
        for (int p = 0; p < group->config.count; p++) {
            if (group->projectiles[p].active) {
                anyProjectileActive = true;
                break;
            }
        }
        if (!anyProjectileActive) {
            group->active = false;
        }
    }
}

void PlayerDamaged(int damage){
    if (player.invulnTimer > 0.0f) return;

    player.hp -= damage;

    if (player.hp <= 0) {
        player.hp = 0;
        gameData->currentState = STATE_GAMEOVER;
    }

    player.invulnTimer = INVUL_TIME;
    player.blinkTimer = 0.0f;
}