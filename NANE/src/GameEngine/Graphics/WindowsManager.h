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
    const int BORDER_WIDTH = 10; //pixels

    //screen resolution
    const int xPixelRes = 1920;
    const int yPixelRes = 1080;

    //SDL window handler
    SDL_Window * gWindow = NULL;
    //SDL local renderer for the window
    SDL_Renderer * gRenderer = NULL;
    //Globally used font
    TTF_Font * gFont = NULL;

    Window mainWindow;
    Window chrRomWindow;
    Window cpuWindow;

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