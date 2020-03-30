#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <iostream>
#include <SDL.h>
#include <memory>

#include "NES/Nes.h"
#include "Window.h"

/**
 * responsible for updating the graphical display of the game.
 */
class WindowManager
{
    private:
    //screen resolution
    const int xPixelRes = 1920;
    const int yPixelRes = 1080;

    //SDL window handler
    SDL_Window * gWindow;
    //SDL local renderer for the window
    SDL_Renderer *gRenderer;

    Window mainWindow;
    Window chrRomWindow;

    public:
    /**
     * Initalizes the graphics engine.
     */
    bool Init();
    
    /**
     * Invoked everytime the screen is ready to be refreshed.
     */
    bool Display(Nes& nesEmulator);

    /**
     * Close the graphics engine.
     */
    void Close();
};

#endif