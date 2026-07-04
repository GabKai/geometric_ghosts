#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "raylib.h"

typedef struct {
    char name[64];    
    Sound sound;      
} CachedSound;

void InitAudioManager(void);
void PlaySFX(const char *soundName);
void UnloadAudioManager(void);

#endif