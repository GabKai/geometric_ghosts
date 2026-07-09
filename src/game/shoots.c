#include "shoots.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../common/audio_manager.h"

static ShootTemplate templates[MAX_SHOOT_TEMPLATES];
static int templateCount = 0;

static ShootGroup activeGroups[MAX_ACTIVE_GROUPS];

static MoveType ParseMoveType(const char *typeStr) {
    if (strcmp(typeStr, "circle") == 0) return MOVE_CIRCLE;
    if (strcmp(typeStr, "linear") == 0) return MOVE_LINEAR;
    if (strcmp(typeStr, "sine") == 0) return MOVE_SINE;
    if (strcmp(typeStr, "splash") == 0) return MOVE_SPLASH;
    return MOVE_LINEAR;
}

void LoadShootTemplates(void) {
    FILE *file = fopen("info/shoots.txt", "r");
    templateCount = 0;
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Nao foi possivel abrir info/shoots.txt");
        return;
    }

    char typeStr[32];
    while (templateCount < MAX_SHOOT_TEMPLATES && 
           fscanf(file, "%s", templates[templateCount].name) == 1) {
        
        fscanf(file, "%s", typeStr);
        templates[templateCount].moveType = ParseMoveType(typeStr);
        
        fscanf(file, " %127[^\n]", templates[templateCount].text);
        fscanf(file, "%f", &templates[templateCount].cooldown);
        fscanf(file, "%d", &templates[templateCount].count);
        fscanf(file, "%d", &templates[templateCount].damage);
        fscanf(file, "%f", &templates[templateCount].duration);
        fscanf(file, "%f", &templates[templateCount].refSpeed);
        fscanf(file, "%s", templates[templateCount].sound);
        fscanf(file, "%f", &templates[templateCount].aux1);
        fscanf(file, "%f", &templates[templateCount].aux2);
        fscanf(file, "%f", &templates[templateCount].aux3);

        //TraceLog(LOG_INFO, "SHOOT TEMPLATE: \n%s\n%s\n%d\n%d\n%d\n%f\n%s\n%f\n%f\n%f", templates[templateCount].name, typeStr, templates[templateCount].count, templates[templateCount].damage, templates[templateCount].duration, templates[templateCount].refSpeed, templates[templateCount].sound, templates[templateCount].aux1, templates[templateCount].aux2, templates[templateCount].aux3);
        
        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "assets/icons/shoots/%s.png", templates[templateCount].name);

        if (FileExists(fullPath)) {
            templates[templateCount].texture = LoadTexture(fullPath);
        }

        char endChar[2];
        fscanf(file, "%s", endChar);

        templateCount++;
    }
    fclose(file);
    
    for (int i = 0; i < MAX_ACTIVE_GROUPS; i++) activeGroups[i].active = false;
}

void SpawnShoot(const char *name, Vector2 origin, Vector2 target, ShooterInfo shooter, bool isAlly) {
    ShootTemplate *targetTemplate = NULL;
    for (int i = 0; i < templateCount; i++) {
        if (strcmp(templates[i].name, name) == 0) {
            targetTemplate = &templates[i];
            break;
        }
    }
  
    if (!targetTemplate) return; 

    int groupIdx = -1;
    for (int i = 0; i < MAX_ACTIVE_GROUPS; i++) {
        if (!activeGroups[i].active) {
            groupIdx = i;
            break;
        }
    }
    if (groupIdx == -1) return;

    ShootGroup *g = &activeGroups[groupIdx];
    g->active = true;
    g->isAlly = isAlly;
    g->config = *targetTemplate;
    g->refPosition = origin;
    g->currentTime = 0.0;

    g->damage = targetTemplate->damage + shooter.bonusDamage;
    g->refSpeed = targetTemplate->refSpeed + shooter.bonusSpeed;
    g->aux1 = g->config.aux1 + shooter.aux1Mod;
    g->aux2 = g->config.aux2 + shooter.aux2Mod;
    g->aux3 = g->config.aux3 + shooter.aux3Mod;

    g->radius = (int) (0.5f + ((float)g->damage * PROJECTILE_SIZE_BASE / 10.0f));

    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float len = sqrtf(dx*dx + dy*dy);
    g->direction = (len > 0) ? (Vector2){ dx/len, dy/len } : (Vector2){ 1, 0 };
    
    g->count = g->config.count;

    switch (g->config.moveType)
    {
    case MOVE_SPLASH:
        g->count += (int) g->aux2;
        break;    
    default:
        break;
    }

    if (g->count > MAX_PROJECTILES_PER_GROUP) g->count = MAX_PROJECTILES_PER_GROUP;

    for (int i = 0; i < g->count; i++) {
        g->projectiles[i].active = true;
        g->projectiles[i].localPosition = origin;

        switch(g->config.moveType){
            case MOVE_LINEAR:
                g->projectiles[i].offset = i * g->aux1;
                break;
            case MOVE_CIRCLE:
                g->projectiles[i].offset = i * (2.0f * PI / g->count);
                break;
            case MOVE_SINE:
                g->projectiles[i].offset = i * PI;
                break;
            case MOVE_SPLASH:
                float baseAngle = atan2f(g->direction.y, g->direction.x);

                float randomPercent = (float)GetRandomValue(0, 100) / 100.0f; 
                float angleOffset = -g->aux1 + randomPercent * (2.0f * g->aux1);

                g->projectiles[i].offset = baseAngle + angleOffset;
                break;
            default:
                break;
        }
    }

    PlaySFX(g->config.sound);
}

void UpdateShoots(void) {
    float deltaTime = GetFrameTime();

    for (int i = 0; i < MAX_ACTIVE_GROUPS; i++) {
        if (!activeGroups[i].active) continue;

        ShootGroup *g = &activeGroups[i];
        g->currentTime += deltaTime;

        if (g->currentTime >= g->config.duration) {
            g->active = false;
            continue;
        }

        g->refPosition.x += g->direction.x * g->refSpeed * deltaTime;
        g->refPosition.y += g->direction.y * g->refSpeed * deltaTime;

        for (int p = 0; p < g->count; p++) {
            if (!g->projectiles[p].active) continue;

            switch(g->config.moveType){
                case MOVE_LINEAR:
                    g->projectiles[p].offset = g->projectiles[p].offset + g->aux2 * g->currentTime * (p + 1);
                    g->projectiles[p].localPosition.x = g->refPosition.x + (g->direction.x * g->projectiles[p].offset);
                    g->projectiles[p].localPosition.y = g->refPosition.y + (g->direction.y * g->projectiles[p].offset);
                    break;
                case MOVE_CIRCLE:
                    float finalAngle = g->currentTime*g->aux1 + g->projectiles[p].offset;
                    float radius = g->aux2 + g->currentTime*g->aux3;
                    
                    g->projectiles[p].localPosition.x = g->refPosition.x + cosf(finalAngle) * radius;
                    g->projectiles[p].localPosition.y = g->refPosition.y + sinf(finalAngle) * radius;
                    break;
                case MOVE_SINE:
                    float ampli = g->aux1 + g->currentTime*g->aux3;
                    float wave = sinf(g->currentTime * g->aux2 + g->projectiles[p].offset) * ampli;

                    Vector2 perp = { -g->direction.y, g->direction.x };

                    g->projectiles[p].localPosition.x = g->refPosition.x + (perp.x * wave);
                    g->projectiles[p].localPosition.y = g->refPosition.y + (perp.y * wave);
                    break;
                case MOVE_SPLASH:
                    float angle = g->projectiles[p].offset; 
    
                    g->projectiles[p].localPosition.x += cosf(angle) * g->refSpeed * deltaTime;
                    g->projectiles[p].localPosition.y += sinf(angle) * g->refSpeed * deltaTime;
                    break;
            }
        }
    }
}

void DrawShoots(void) {
    for (int i = 0; i < MAX_ACTIVE_GROUPS; i++) {
        if (!activeGroups[i].active) continue;

        ShootGroup *g = &activeGroups[i];
        Color shootColor = g->isAlly ? BLUE : RED;

        for (int p = 0; p < g->count; p++) {
            if (!g->projectiles[p].active) continue;
            
            DrawCircleV(g->projectiles[p].localPosition, g->radius, shootColor);
            DrawCircleV(g->projectiles[p].localPosition, g->radius / 2, WHITE);
        }
    }
}

int GetActiveShootGroupsCount(void) {
    return MAX_ACTIVE_GROUPS;
}

ShootGroup* GetShootGroupInstance(int index) {
    if (index >= 0 && index < MAX_ACTIVE_GROUPS && activeGroups[index].active) {
        return &activeGroups[index];
    }
    return NULL;
}

ShootTemplate* GetShootTemplate(const char *name) {
    if (name == NULL) return NULL;

    for (int i = 0; i < templateCount; i++) {
        if (strcmp(templates[i].name, name) == 0) {
            return &templates[i]; 
        }
    }

    return NULL; 
}

ShootTemplate* GetRandomShootTemplate(void) {
    int idx = GetRandomValue(0, templateCount - 1);
    return &templates[idx];
}