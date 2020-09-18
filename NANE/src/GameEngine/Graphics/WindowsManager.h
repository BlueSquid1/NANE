#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include <iostream>
#include <SDL.h>
#include <memory>
#include <sstream>

#include "NES/Nes.h"
#include "TextureWindow.h"
#include "TextWindow.h"
#include "MenuBar.h"
#include "FontManager.h"
#include "FileNavigator.h"

enum class WindowView
{
    Simple,
    Disassemble,
    OpenRom
};


/**
 * responsible for updating the graphical display of the game.
 */
class WindowManager
{
    int windowScale = 1;
    bool enableMenuBar = true;
    WindowView curWindowView = WindowView::Disassemble;
    bool enableFpsCounter = true;

    //SDL window handler
    SDL_Window * gWindow = NULL;
    //SDL local renderer for the window
    SDL_Renderer * gRenderer = NULL;

    std::unique_ptr<TextureWindow> mainWindow;
    std::unique_ptr<TextureWindow> chrRomWindow;
    std::unique_ptr<TextWindow> cpuWindow;
    std::unique_ptr<TextureWindow> colourDisplayWindow;
    std::unique_ptr<TextWindow> fpsDisplay;
    std::unique_ptr<TextureWindow> playerOneInputs;
    std::unique_ptr<MenuBar> menuBar;
    std::unique_ptr<FileNavigator> openRomWindow;

    std::stringstream fpsStringStream;

    void UpdateLayout();

    void RenderMainDisplay(Nes& nesEmulator,unsigned int fps);

    public:
    /**
     * Initalizes the graphics engine.
     */
    bool Init(bool showFileMenu = true);
    
    void ChangeScaleFactor(int newScaleFactor);

    void ChangeViewType(WindowView newWindowView);

    void HandleEvent(const SDL_Event& e);
    
    /**
     * Invoked everytime the screen is ready to be refreshed.
     */
    bool Display(Nes& nesEmulator,unsigned int fps);

    /**
     * Close the graphics engine.
     */
    void Close();

    //getters/setters
    std::unique_ptr<FileNavigator>& GetFileNavigator();
};

#endif