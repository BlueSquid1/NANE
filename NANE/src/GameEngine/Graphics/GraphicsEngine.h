#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <iostream>
#include <SDL.h>

class GraphicsEngine
{
    const int xPixelRes = 1920;
    const int yPixelRes = 1080;

    SDL_Window * gWindow;
    SDL_Renderer *gRenderer;

    public:
    bool Init();
    bool Display();
    void Close();
};

#endif