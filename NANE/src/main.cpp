#include <iostream> //cout
#include <string>
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
        std::cerr << "failed to initalizing game engine" << std::endl;
		return 1;
    }

    if(argc == 2)
    {
        std::string romPath = argv[1];
        std::cout << "loading rom: " << romPath << std::endl;
        if(gameEngine.LoadMedia(romPath) == false)
        {
            std::cerr << "failed to load media" << std::endl;
            return 1;
        }
    }
    else if(argc == 4)
    {
        std::string romPath = argv[1];
        std::cout << "loading rom: " << romPath << std::endl;
        if(gameEngine.LoadMedia(romPath) == false)
        {
            std::cerr << "failed to load media" << std::endl;
            return 1;
        }

        if(argv[2] == std::string("-d"))
        {
            int lineNum = std::stoi(argv[3], nullptr, 16);
            gameEngine.SetDebugPoint(lineNum);
        }
    }

    std::cout << "Entering main loop" << std::endl;
    while (gameEngine.GetShouldExit() == false)
    {
        bool inputRet = gameEngine.UserInput();
        if(inputRet == false)
        {
            std::cerr << "failed to read user input" << std::endl;
            return 1;
        }

        bool processRet = gameEngine.Processing();
        if(processRet == false)
        {
            std::cerr << "processing failed" << std::endl;
            return 1;
        }

        bool displayRet = gameEngine.Display();
        if(displayRet == false)
        {
            std::cerr << "displaying failed" << std::endl;
            return 1;
        }
    }

    gameEngine.Close();
    return 0;
}
