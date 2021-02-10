#pragma once

#include <SDL.h>
#include <vector>
#include <memory>

#include "NES/Memory/ThreadSafeQueue.h"

class AudioMgr
{
private:
    const int samplesPerSecond = 44100;

    SDL_AudioDeviceID audioId;
    std::shared_ptr<ThreadSafeQueue<float>> audioQueue;

    static void SdlAudioPlaybackCallback( void* userdata, Uint8* stream, int len );

    void ConsumeAudio(float * stream, unsigned int len);
public:
    bool Init();
    bool PlayAudio(std::shared_ptr<ThreadSafeQueue<float>> audioStream);

    int GetSamplesPerSecond();
};