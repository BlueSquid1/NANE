#include "GameEngine.h"


#include <SDL.h> //SDL_PollEvent()

GameEngine::GameEngine()
{

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

	bool isGraphicsInit = this->windowsMgr.Init();
	if(isGraphicsInit == false)
	{
		std::cerr << "failed to initalize graphics engine" << std::endl;
		return false;
	}

	return true;
}

bool GameEngine::LoadMedia()
{
	std::string romPath = "./NANE/test/resources/nestest.nes";
	//std::string romPath = "./NANE/test/resources/DonkeyKong.nes";
	if(this->nesEmulator.LoadCartridge(romPath) == false)
	{
		return false;
	}

	return true;
}

bool GameEngine::PostInit()
{
	if(this->nesEmulator.PowerCycle() == false)
	{
		return false;
	}
	return true;
}

bool GameEngine::UserInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
        if (e.type == SDL_QUIT)
		{
            this->shouldExit = true;
        }
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
		{
			if(e.type == SDL_KEYDOWN)
			{
				//key down

				//special cases
				switch( e.key.keysym.sym )
				{
					case SDLK_n:
					{
						this->step = true;
						break;
					}
					case SDLK_p:
					{
						this->run = !this->run;
						break;
					}
					case SDLK_c:
					{
						this->nesEmulator.IncrementDefaultColourPalette();
						break;
					}
					default:
					{
						NesController::NesInputs nesInput = keyMapper.ToNesInput(e.key.keysym.sym);
						this->nesEmulator.PressButton(nesInput, true);
					}
				}
			}
			else
			{
				//key up
				NesController::NesInputs nesInput = keyMapper.ToNesInput(e.key.keysym.sym);
				this->nesEmulator.PressButton(nesInput, false);
			}
        }
    }
	return true;
}

bool GameEngine::Processing()
{
	if(this->step == true || this->run == true)
	{
		this->nesEmulator.processes(this->verbose);
		this->step = false;
	}
	return true;
}

bool GameEngine::Display()
{
	unsigned int fps = fpsTimer.CalcFps();
	bool showDisassembly = true;
	bool graphicsRet = this->windowsMgr.Display(this->nesEmulator, fps, showDisassembly);
	if(graphicsRet == false)
	{
		std::cerr << "graphics failed to display" << std::endl;
	}
	return true;
}

void GameEngine::Close()
{
	std::cout << "closing game engine" << std::endl;
	this->windowsMgr.Close();
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