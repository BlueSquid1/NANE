#pragma once

#include <SDL.h>

class AudioMgr
{
private:
    static int samplesPerSecond;
    static float secondsPerSample;

    static float timeElapsed;
public:
    bool Init();
    static void AudioPlaybackCallback( void* userdata, Uint8* stream, int len );
};