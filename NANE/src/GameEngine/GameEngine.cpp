#include "GameEngine.h"

GameEngine::GameEngine()
{
	this->graphics = std::unique_ptr<GraphicsEngine>(new GraphicsEngine());
	this->nesEmulator = std::unique_ptr<Nes>(new Nes());
}


bool GameEngine::Init(const std::string & serverIp)
{
    bool isLogInit = Logger::Initalize(serverIp, STDOUT_FILENO, STDERR_FILENO);
	if(isLogInit == false)
	{
		std::cerr << "failed to initalize network logger" << std::endl;
		return false;
	}

	std::cout << "initalizing game engine" << std::endl;

	bool isGraphicsInit = this->graphics->Init();
	if(isGraphicsInit == false)
	{
		std::cerr << "failed to initalize graphics engine" << std::endl;
		return false;
	}

	return true;
}

bool GameEngine::LoadMedia()
{
	std::string romPath = "./NANE-Desktop/build/nestest.nes";
	if(this->nesEmulator->LoadCartridge(romPath) == false)
	{
		return false;
	}

	return true;
}

bool GameEngine::PostInit()
{
	if(this->nesEmulator->PowerCycle() == false)
	{
		return false;
	}

	//TODO delete
	this->nesEmulator->processes();

	return true;
}

bool GameEngine::UserInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            this->shouldExit = true;
        }
        if (e.type == SDL_KEYDOWN){
            this->shouldExit = true;
        }
    }
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
		std::cerr << "graphics failed to display" << std::endl;
	}
	return true;
}

void GameEngine::Close()
{
	std::cout << "closing game engine" << std::endl;
	this->graphics->Close();
	Logger::Close();
}


//getter and setters

bool GameEngine::GetShouldExit()
{
	#ifdef __SWITCH__
	bool shouldContinue = appletMainLoop();
	if(shouldContinue == false)
	{
		return true;
	}
	#endif
	return this->shouldExit;
}