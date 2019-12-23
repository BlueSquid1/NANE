#include "GameEngine.h"

GameEngine::GameEngine()
{
	this->graphics = std::unique_ptr<GraphicsEngine>(new GraphicsEngine());
}


bool GameEngine::Init(std::string serverIp)
{
	std::cout << "initalizing game engine." << std::endl;
    bool isLogInit = Logger::Initalize(serverIp, STDOUT_FILENO, STDERR_FILENO);
	if(isLogInit == false)
	{
		std::cout << "failed to initalize network logger." << std::endl;
		return false;
	}

	bool isGraphicsInit = this->graphics->Init();
	if(isGraphicsInit == false)
	{
		std::cout << "failed to initalize graphics engine." << std::endl;
		return false;
	}
	return true;
}

bool GameEngine::UserInput()
{
	return true;
}

bool GameEngine::Processing()
{
	return true;
}

bool GameEngine::Display()
{
	bool graphicsRet = this->graphics->Display();
	if(graphicsRet == false)
	{
		std::cout << "graphics failed to display." << std::endl;
	}
	return true;
}

void GameEngine::Close()
{
	std::cout << "closing game engine." << std::endl;
	this->graphics->Close();
	Logger::Close();
}


//getter and setters

bool GameEngine::GetShouldExit()
{
	return this->shouldExit;
}