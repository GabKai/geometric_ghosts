#include "audio_manager.h"
#include <string.h>
#include <stdio.h>

#define MAX_CACHED_SOUNDS 32

static CachedSound soundCache[MAX_CACHED_SOUNDS];
static int cachedCount = 0;
static float volume = 1.0f;

void InitAudioManager(void) {
    InitAudioDevice(); 
    cachedCount = 0;
    
    FILE *fileOpts = fopen("info/options.txt", "r");
    if (fileOpts != NULL) {
        fscanf(fileOpts, "%f", &volume);
        fclose(fileOpts);
    }
        
    SetMasterVolume(volume);
}

void PlaySFX(const char *soundName) {
    Sound soundToPlay;
    bool found = false;

    for (int i = 0; i < cachedCount; i++) {
        if (strcmp(soundCache[i].name, soundName) == 0) {
            soundToPlay = soundCache[i].sound;
            found = true;
            break;
        }
    }

    if (!found) {
        if (cachedCount >= MAX_CACHED_SOUNDS) {
            TraceLog(LOG_WARNING, "AudioManager: Buffer cheio! Nao foi possivel carregar %s", soundName);
            return;
        }

        char fullPath[256];
        snprintf(fullPath, sizeof(fullPath), "assets/sounds/%s", soundName);

        if (!FileExists(fullPath)) {
            TraceLog(LOG_ERROR, "AudioManager: Arquivo nao encontrado em %s", fullPath);
            return;
        }

        soundToPlay = LoadSound(fullPath);
        strncpy(soundCache[cachedCount].name, soundName, 63);
        soundCache[cachedCount].name[63] = '\0';
        soundCache[cachedCount].sound = soundToPlay;
        cachedCount++;
        
        TraceLog(LOG_INFO, "AudioManager: '%s' carregado no buffer.", soundName);
    }

    float randomPitch = GetRandomValue(80, 120) / 100.0f;
    
    SetSoundPitch(soundToPlay, randomPitch);
    PlaySound(soundToPlay);
}

void UnloadAudioManager(void) {
    for (int i = 0; i < cachedCount; i++) {
        UnloadSound(soundCache[i].sound);
    }
    cachedCount = 0;
    CloseAudioDevice();
}