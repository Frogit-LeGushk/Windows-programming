#include "WAVHandlers.h"

static long last_cloak = 0;

void playFrogCroak(TState *state) {
    if(clock() - last_cloak > 1000) {
        PlaySoundA(TEXT("04479.wav"), NULL, SND_ASYNC);
        last_cloak = clock();
    }
}
