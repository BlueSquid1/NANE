#include "AudioMgr.h"
#include <iostream>

bool AudioMgr::Init()
{
	SDL_AudioSpec wantedAudio;
	SDL_memset(&wantedAudio, 0, sizeof(wantedAudio));
	wantedAudio.freq = samplesPerSecond;
	wantedAudio.format = AUDIO_F32;
	wantedAudio.channels = 1;
	wantedAudio.samples = 4096;
	wantedAudio.callback = AudioMgr::SdlAudioPlaybackCallback;
    wantedAudio.userdata = this;

	SDL_AudioSpec obtainedAudio;
	this->audioId = SDL_OpenAudioDevice(nullptr, SDL_FALSE, &wantedAudio, &obtainedAudio, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if(this->audioId <= 0)
    {
        return false;
    }
    return true;
}

void AudioMgr::SdlAudioPlaybackCallback(void* userdata, Uint8* stream, int len )
{
    int bytesPerSample = 4;
    int floatLen = len / bytesPerSample;
    float* floatStream = reinterpret_cast<float*>(stream);

    AudioMgr * audioObject = (AudioMgr *)userdata;
    audioObject->ConsumeAudio(floatStream, floatLen);
}

void AudioMgr::ConsumeAudio(float * stream, unsigned int len)
{
    std::vector<float> audioData = this->audioQueue->Pop(len);
    for(unsigned int i = 0; i < audioData.size(); ++i)
    {
        stream[i] = audioData.at(i);
    }

    if(audioData.size() < len)
    {
        std::cerr << "consuming audio faster than it is produced" << std::endl;

        //pad rest with zero
        for(unsigned int i = audioData.size(); i < len; ++i)
        {
            stream[i] = 0;
        }
    }
}

bool AudioMgr::PlayAudio(std::shared_ptr<ThreadSafeQueue<float>> audioStream)
{
    this->audioQueue = audioStream;

    /* start audio */
	int pauseOn = SDL_FALSE;
	SDL_PauseAudioDevice(this->audioId, pauseOn);
    return true;
}

int AudioMgr::GetSamplesPerSecond()
{
    return this->samplesPerSecond;
}