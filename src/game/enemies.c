#include "enemies.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static EnemyTemplate enemyTemplates[MAX_ENEMY_TEMPLATES];
static int enemyTemplateCount;
static int rarityTemplateCount[] = {0,0,0};

static Enemy activeEnemies[MAX_ACTIVE_ENEMIES];

static float spawnTimer = 0.0f;

void LoadEnemyTemplates(void) {
    FILE *file = fopen("info/enemies.txt", "r");
    enemyTemplateCount = 0;
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Nao foi possivel abrir info/enemies.txt");
        return;
    }

    char word[64];
    while (enemyTemplateCount < MAX_ENEMY_TEMPLATES && 
           fscanf(file, "%s", enemyTemplates[enemyTemplateCount].name) == 1) {
        
        EnemyTemplate *t = &enemyTemplates[enemyTemplateCount];
        
        fscanf(file, "%d", &t->rarity);
        fscanf(file, "%d", &t->size);
        fscanf(file, "%d", &t->maxHp);
        fscanf(file, "%d", &t->damage);
        fscanf(file, "%f", &t->speed);
        fscanf(file, "%f", &t->minDist);
        fscanf(file, "%f", &t->shootCooldown);
        
        t->hasShoot = false;
        t->hasDeath = false;

        while (fscanf(file, "%s", word) == 1) {
            if (strcmp(word, ";") == 0) break; 

            if (strcmp(word, "tiro:") == 0) {
                t->hasShoot = true;
                fscanf(file, "%s", t->shootTemplateName);
                fscanf(file, "%f %f %f %f %d", &t->shootShooterInfo.aux1Mod, &t->shootShooterInfo.aux2Mod, &t->shootShooterInfo.aux3Mod, &t->shootShooterInfo.bonusSpeed, &t->shootShooterInfo.bonusDamage);
            }else if (strcmp(word, "morte:") == 0) {
                t->hasDeath = true;
                fscanf(file, "%s", t->deathTemplateName);
                fscanf(file, "%f %f %f %f %d", &t->deathShooterInfo.aux1Mod, &t->deathShooterInfo.aux2Mod, &t->deathShooterInfo.aux3Mod, &t->deathShooterInfo.bonusSpeed, &t->deathShooterInfo.bonusDamage);
            }
        }

        //TraceLog(LOG_INFO, "ENEMIES: template carregado \n%s\n%d\n%d\n%d\n%f\n%f\n%f",t->name, t->rarity, t->maxHp, t->damage, t->speed, t->minDist, t->shootCooldown);

        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "assets/sprites/enemies/%s.png", t->name);

        if (FileExists(fullPath)) {
            t->texture = LoadTexture(fullPath);
            t->textureLoaded = true;
        } else {
            t->textureLoaded = false;
            //TraceLog(LOG_WARNING, "Enemies: Sprite nao encontrado em %s", fullPath);
        }

        enemyTemplateCount++;
        rarityTemplateCount[t->rarity]++;
    }
    fclose(file);
    
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++) activeEnemies[i].active = false;
    TraceLog(LOG_INFO, "Enemies: %d templates carregados com sucesso.", enemyTemplateCount);
}

void SpawnEnemy(Vector2 playerPos) {
    if (enemyTemplateCount == 0) return;

    spawnTimer += GetFrameTime();
    if (spawnTimer >= 2.0f) {
        spawnTimer = 0.0f;
    }else{
        return;
    }

    int idx = -1;
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++) {
        if (!activeEnemies[i].active) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return;

    int rarity = 0;
    if (GetRandomValue(0, 1) == 1){
        rarity = 1;
        if (GetRandomValue(0, 2) == 2){
            rarity = 2;
        }
    }

    while(rarityTemplateCount[rarity] == 0){
        rarity--;

        if (rarity < 0){
            return;
        }
    }

    int randTemplate = GetRandomValue(1, rarityTemplateCount[rarity]);

    int idxTemplate = 0;
    for (int i = 0; i < enemyTemplateCount; i++) {
        if (enemyTemplates[i].rarity == rarity) {
            randTemplate--;
            if (randTemplate <= 0) {
                idxTemplate = i;
                break;
            }
        }
    }
    
    Enemy *e = &activeEnemies[idx];
    e->active = true;
    e->config = enemyTemplates[idxTemplate];
    e->hp = e->config.maxHp;
    float randomPercent = GetRandomValue(0, 100) / 100.0f;
    e->cooldownTimer = randomPercent * e->config.shootCooldown;

    float offsetX = 2000.0f;
    float offsetY = 1500.0f;

    if (GetRandomValue(0, 1) == 0) {
        float dx = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
        e->position.x = playerPos.x + (dx * offsetX);
        e->position.y = playerPos.y + (float)GetRandomValue((int)-offsetY, (int)offsetY);
    } else {
        float dy = (GetRandomValue(0, 1) == 0) ? -1.0f : 1.0f;
        e->position.y = playerPos.y + (dy * offsetY);
        e->position.x = playerPos.x + (float)GetRandomValue((int)-offsetX, (int)offsetX);
    }
}

void UpdateEnemies(Vector2 playerPos) {
    float deltaTime = GetFrameTime();

    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++) {
        if (!activeEnemies[i].active) continue;

        Enemy *e = &activeEnemies[i];

        if (e->stunTimer > 0.0f) {
            e->stunTimer -= deltaTime;
            continue; 
        }

        float dx = playerPos.x - e->position.x;
        float dy = playerPos.y - e->position.y;
        float dist = sqrtf(dx*dx + dy*dy);

        if (dist > 0) {
            Vector2 dir = { dx / dist, dy / dist };

            if (dist > e->config.minDist) {
                e->position.x += dir.x * e->config.speed * deltaTime;
                e->position.y += dir.y * e->config.speed * deltaTime;
            } else if (dist < e->config.minDist - 20.0f) {
                e->position.x -= dir.x * (e->config.speed * 0.5f) * deltaTime;
                e->position.y -= dir.y * (e->config.speed * 0.5f) * deltaTime;
            }
        }

        if (!e->config.hasShoot) { continue; }

        e->cooldownTimer += deltaTime;
        if (e->cooldownTimer >= e->config.shootCooldown) {
            e->cooldownTimer = 0;
            
            SpawnShoot(e->config.shootTemplateName, e->position, playerPos, e->config.shootShooterInfo, false);
        }
    }
}

void DrawEnemies(void) {
    for (int i = 0; i < MAX_ACTIVE_ENEMIES; i++) {
        if (!activeEnemies[i].active) continue;

        Enemy *e = &activeEnemies[i];

        if (e->config.textureLoaded) { 
            Rectangle sourceRec = { 0.0f, 0.0f, (float)e->config.texture.width, (float)e->config.texture.height };
            Rectangle destRec = { e->position.x, e->position.y, e->config.size, e->config.size };
            Vector2 origin = { e->config.size / 2.0f, e->config.size / 2.0f }; 
            
            DrawTexturePro(e->config.texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }else{        
            DrawCircleV(e->position, 18, MAROON);
            DrawCircleV(e->position, 15, RED);
        }
        
        DrawRectangle(e->position.x - 15, e->position.y - 25, 30, 4, BLACK);
        float hpPercent = (float)e->hp / e->config.maxHp;
        DrawRectangle(e->position.x - 15, e->position.y - 25, (int)(30 * hpPercent), 4, GREEN);
    }
}

int GetActiveEnemiesCount(void) {
    return MAX_ACTIVE_ENEMIES;
}

Enemy* GetEnemyInstance(int index) {
    if (index >= 0 && index < MAX_ACTIVE_ENEMIES && activeEnemies[index].active) {
        return &activeEnemies[index];
    }
    return NULL;
}