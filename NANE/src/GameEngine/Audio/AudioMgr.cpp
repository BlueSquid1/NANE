#include <math.h>

#include "AudioMgr.h"
#include <iostream>

#define PI 3.14159265

int AudioMgr::samplesPerSecond = 44100;
float AudioMgr::secondsPerSample = 1.0f/samplesPerSecond;

float AudioMgr::timeElapsed = 0.0f;

bool AudioMgr::Init()
{
    //TODO
	SDL_AudioSpec wantedAudio;
	SDL_memset(&wantedAudio, 0, sizeof(wantedAudio));
	wantedAudio.freq = samplesPerSecond;
	wantedAudio.format = AUDIO_F32;
	wantedAudio.channels = 1;
	wantedAudio.samples = 4096;
	wantedAudio.callback = AudioMgr::AudioPlaybackCallback;

	SDL_AudioSpec obtainedAudio;

	SDL_AudioDeviceID audioId = SDL_OpenAudioDevice(nullptr, SDL_FALSE, &wantedAudio, &obtainedAudio, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(audioId <= 0)
    {
        return false;
    }

	int pause_on = SDL_FALSE;
	SDL_PauseAudioDevice(audioId, pause_on);
    return true;
}

void AudioMgr::AudioPlaybackCallback( void* userdata, Uint8* stream, int len )
{
    int bytesPerSample = 4;
    int floatLen = len / bytesPerSample;
    float* floatStream = reinterpret_cast<float*>(stream);

    float frequency = 500; //Hz

    for(int i = 0; i < floatLen; ++i)
    {
        float sampleValue = 0.3 * sin(frequency * timeElapsed * 2 * PI);

        floatStream[i] = sampleValue;

        timeElapsed += secondsPerSample;
    }
}