#include "MY_ASSERT.h"
#include <stdlib.h>

#include "sound.h"

struct SoundManager *initSoundManager(void) {
    struct SoundManager *result = malloc(sizeof(struct SoundManager));
    *result = (struct SoundManager){
        .engine = calloc(1, sizeof(ma_engine)),
        .qSound = 0,
        .sound = NULL
    };

    MY_ASSERT(MA_SUCCESS == ma_engine_init(NULL, result->engine));

    return result;
}

void loadSound(struct SoundManager *this, size_t soundIndex, const char *soundName) {
    if (this->qSound <= soundIndex) {
        if (this->sound) {
            this->sound = realloc(this->sound, sizeof(ma_sound *) * (soundIndex + 1));

            for (size_t i = this->qSound; i < soundIndex + 1; i += 1) {
                this->sound[i] = NULL;
            }
        }
        else {
            this->sound = calloc(soundIndex + 1, sizeof(ma_sound *));
        }

        this->qSound = soundIndex + 1;
    }

    this->sound[soundIndex] = malloc(sizeof(ma_sound));
    MY_ASSERT(MA_SUCCESS == ma_sound_init_from_file(this->engine, soundName, 0, NULL, NULL, this->sound[soundIndex]));
}

void stopPrevSound(struct SoundManager *this) {
    if (this->prevSound != NULL) {
        ma_sound_stop(this->prevSound);
        ma_sound_seek_to_pcm_frame(this->prevSound, 0);
    }
}

void playSound(struct SoundManager *this, size_t soundIndex, bool shouldLoop, float volume) {
    ma_sound_start(this->sound[soundIndex]);
    ma_sound_set_volume(this->sound[soundIndex], volume);
    ma_sound_set_looping(this->sound[soundIndex], shouldLoop);
    this->prevSound = this->sound[soundIndex];
}

void cleanupSoundManager(void *thisPtr) {
    struct SoundManager *this = thisPtr;
    for (size_t i = 0; i < this->qSound; i += 1) {
        ma_sound_uninit(this->sound[i]);
        free(this->sound[i]);
    }

    ma_engine_uninit(this->engine);

    free(this->sound);
    free(this->engine);
    free(this);
}
