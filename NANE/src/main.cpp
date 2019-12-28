#include <iostream> //cout string
#include "GameEngine/GameEngine.h"

/**
 * program entry point
 */
int main(int argc, char *argv[])
{    
    std::string logServerIp = std::string();
    #ifdef LOG_SERVER_IP
    //if set will send cout and cerr logs to the Log server ip address below
    logServerIp = LOG_SERVER_IP;
    #endif
    
    GameEngine gameEngine;
    if(gameEngine.Init(logServerIp) == false)
    {
        std::cout << "failed to initalizing game engine" << std::endl;
		return 1;
    }

    std::cout << "Entering main loop" << std::endl;
    while (gameEngine.GetShouldExit() == false)
    {
        bool inputRet = gameEngine.UserInput();
        if(inputRet == false)
        {
            std::cout << "failed to read user input" << std::endl;
            return 1;
        }

        bool processRet = gameEngine.Processing();
        if(processRet == false)
        {
            std::cout << "processing failed" << std::endl;
            return 1;
        }

        bool displayRet = gameEngine.Display();
        if(displayRet == false)
        {
            std::cout << "displaying failed" << std::endl;
            return 1;
        }
    }

    gameEngine.Close();
    return 0;
}
