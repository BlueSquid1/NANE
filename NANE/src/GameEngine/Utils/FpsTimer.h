#pragma once

#include <stdint.h>

class FpsTimer
{
    private:
    uint32_t startTime = 0;
    int fps = 0;
    int counter = 0;

    public:
    /**
     * @return the fps from the last second
     */
    unsigned int CalcFps();
};