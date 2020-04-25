#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <iostream>
#include <SDL.h>
#include <memory>
#include <sstream>

#include "NES/Nes.h"
#include "TextureWindow.h"
#include "TextWindow.h"

/**
 * responsible for updating the graphical display of the game.
 */
class WindowManager
{
    private:
    const int BORDER_WIDTH = 5; //pixels
    int windowScale = 2;

    //SDL window handler
    SDL_Window * gWindow = NULL;
    //SDL local renderer for the window
    SDL_Renderer * gRenderer = NULL;
    //Globally used font
    TTF_Font * gFont = NULL;

    TextureWindow mainWindow;
    TextureWindow chrRomWindow;
    TextWindow cpuWindow;
    TextureWindow colourDisplayWindow;
    TextWindow fpsDisplay;
    TextureWindow playerOneInputs;
    std::stringstream fpsStringStream;

    public:
    /**
     * Initalizes the graphics engine.
     */
    bool Init();
    
    void ChangeScaleFactor(int newScaleFactor);
    
    /**
     * Invoked everytime the screen is ready to be refreshed.
     */
    bool Display(Nes& nesEmulator,unsigned int fps, bool showDisassembly);

    /**
     * Close the graphics engine.
     */
    void Close();
};

#endif