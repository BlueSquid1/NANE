#include <iostream> //cout string

#include <switch.h>

#include "GameEngine.h"


int main(int argc, char *argv[])
{    
    GameEngine gameEngine;
    std::string logServerIp = "10.48.119.4";
    if(gameEngine.Init(logServerIp) == false)
    {
        std::cout << "failed to initalizing game engine." << std::endl;
		return 1;
    }

    // Main loop
    while (appletMainLoop() && gameEngine.GetShouldExit() == false)
    {
        bool inputRet = gameEngine.UserInput();
        if(inputRet == false)
        {
            std::cout << "failed to read user input." << std::endl;
            return 1;
        }

        bool processRet = gameEngine.Processing();
        if(processRet == false)
        {
            std::cout << "processing failed." << std::endl;
            return 1;
        }

        bool displayRet = gameEngine.Display();
        if(displayRet == false)
        {
            std::cout << "displaying failed." << std::endl;
            return 1;
        }
    }

    gameEngine.Close();
    return 0;
}
