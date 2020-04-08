#include "FpsTimer.h"

#include <SDL.h>

unsigned int FpsTimer::CalcFps()
{
    uint32_t curTime = SDL_GetTicks();
    if(curTime - this->startTime > 1000)
    {
        //1 second has elapse
        this->fps = counter;
        this->startTime = curTime;
        counter = 0;
    }

    ++counter;
    return this->fps;
}
