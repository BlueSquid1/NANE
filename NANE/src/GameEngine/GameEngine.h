#ifndef GAME_ENGINE
#define GAME_ENGINE

#include "Network/Logger.h"
#include "Graphics/WindowsManager.h"
#include "NES/Nes.h"
#include "Utils/FpsTimer.h"
#include "Input/KeyMapper.h"
#include "Storage/RomLoader.h"

#include <iostream> //std::string
#include <memory> //std::unique_ptr

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
    KeyMapper keyMapper;
    RomLoader romLoader;

    //when set to true the game will close shortly afterwards
    bool shouldExit = false;

    bool frameStep = false;
    bool assemblyStep = false;
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
     * @param romFilePath path to a nes ROM.
     * @return false if a fatal error has occured.
     */
    bool LoadMedia( const std::string& romFilePath );

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