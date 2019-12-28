#ifndef GAME_ENGINE
#define GAME_ENGINE

#include <iostream> //std::string
#include <memory> //std::unique_ptr
#include <SDL.h> //graphics library

#include "Network/Logger.h"
#include "Graphics/GraphicsEngine.h"

/**
 * Responsible for controlling the logic flow from initialization to the game loop to closing the game.
 */
class GameEngine
{
    private:
    //graphics module
    std::unique_ptr<GraphicsEngine> graphics;

    //when set to true the game will close shortly afterwards
    bool shouldExit = false;

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