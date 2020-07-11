#include "GameEngine.h"

#include "Graphics/CustomEventMgr.h"

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
	bool showFileMenu;
#ifdef __DESKTOP__
	showFileMenu = true;
#else
	showFileMenu = false;
#endif

	bool isGraphicsInit = this->windowsMgr.Init(showFileMenu);
	if(isGraphicsInit == false)
	{
		std::cerr << "failed to initalize graphics engine" << std::endl;
		return false;
	}

	return true;
}

bool GameEngine::LoadMedia()
{
	//std::string romPath = "./NANE/test/resources/nestest.nes";
	std::string romPath = "./NANE/test/resources/DonkeyKong.nes";
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
		this->windowsMgr.HandleEvent(e);
		if(e.type == CustomEventMgr::GetInstance()->GetCustomEventType())
		{
			MenuEvents menuEvent = (MenuEvents)e.user.code;
			switch(menuEvent)
			{
				case MenuEvents::ContinuePauseEmulator:
				{
					this->run = !this->run;
					break;
				}
				case MenuEvents::StepEmulator:
				{
					this->frameStep = true;
					break;
				}
				case MenuEvents::IncrementDefaultColourPalette:
				{
					this->nesEmulator.IncrementDefaultColourPalette();
					break;
				}
				case MenuEvents::StepAssembly:
				{
					this->assemblyStep = true;
					break;
				}
			}
		}

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
						this->frameStep = true;
						break;
					}
					case SDLK_p:
					{
						this->run = !this->run;
						break;
					}
					case SDLK_m:
					{
						this->assemblyStep = true;
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
				//key released
				NesController::NesInputs nesInput = keyMapper.ToNesInput(e.key.keysym.sym);
				this->nesEmulator.PressButton(nesInput, false);
			}
        }
    }
	return true;
}

bool GameEngine::Processing()
{
	if(this->frameStep == true || this->run == true || this->assemblyStep == true)
	{
		this->nesEmulator.processes(this->verbose, this->assemblyStep);
		this->frameStep = false;
		this->assemblyStep = false;
	}
	return true;
}

bool GameEngine::Display()
{
	unsigned int fps = fpsTimer.CalcFps();
	bool graphicsRet = this->windowsMgr.Display(this->nesEmulator, fps);
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