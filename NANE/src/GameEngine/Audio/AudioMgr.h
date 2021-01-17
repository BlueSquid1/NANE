#pragma once

#include <SDL.h>

class AudioMgr
{
    public:
    bool Init();
    void audioPlaybackCallback( void* userdata, Uint8* stream, int len );
};