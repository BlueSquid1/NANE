#ifndef GAME_ENGINE
#define GAME_ENGINE

#include "Network/Logger.h"
#include "Graphics/WindowsManager.h"
#include "NES/Nes.h"
#include "Utils/FpsTimer.h"

#include <iostream> //std::string
#include <memory> //std::unique_ptr
#include <SDL.h> //graphics library

/**
 * Responsible for controlling the logic flow from initialization to the game loop to closing the game.
 */
class GameEngine
{
    private:
    //graphics module
    WindowManager windowsMgr;
    Nes nesEmulator;
    FpsTimer fpsTimer;

    //when set to true the game will close shortly afterwards
    bool shouldExit = false;

    bool step = false;
    bool run = false;
    bool verbose = false;

    public:
    /**
     * constructor
     */
    GameEngine();

    /**
     * Initalises all submodules. This module should be called once before using any other method.
     * @param serverIp the logging server ip. If set to empty string then network logging is disabled.
     * @return false if a fatal error has occured.
     */
    bool Init(const std::string & serverIp);
    
    /**
     * Handles user inputs in the game loop
     * @return false if a fatal error has occured.
     */
    bool UserInput();

    /**
     * Handles loading emulator media
     * @return false if a fatal error has occured.
     */
    bool LoadMedia();

    /**
     * Final initalisation before entering game loop
     * @return false if a fatal error has occured.
     */
    bool PostInit();

    /**
     * Handles any proccessing that needs to take place in the game loop.
     * @return false if a fatal error has occured.
     */
    bool Processing();

    /**
     * Handles updating the display.
     * @return false if a fatal error has occured.
     */
    bool Display();

    /**
     * Closes all game engine modules after the game loop.
     */
    void Close();

    //getter and setters
    
    bool GetShouldExit();
};

#endif