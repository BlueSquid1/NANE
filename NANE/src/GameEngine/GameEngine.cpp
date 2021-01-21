#include "GameEngine.h"

#include <SDL.h> //SDL_PollEvent()
#include <vector>

#include "Graphics/CustomEventMgr.h"

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
	std::string defaultRomPath;

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

	bool isAudioInit = this->audioMgr.Init();
	if(isAudioInit == false)
	{
		std::cerr << "failed to initalize audio manager: " <<  SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool GameEngine::LoadMedia( const std::string& romFilePath )
{
	if(this->nesEmulator.LoadCartridge(romFilePath) == false)
	{
		return false;
	}

	if(this->nesEmulator.PowerCycle() == false)
	{
		return false;
	}

	// run the rom
	this->run = true;

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
			CustomEvents menuEvent = (CustomEvents)e.user.code;
			switch(menuEvent)
			{
				case CustomEvents::ContinuePauseEmulator:
				{
					if(this->nesEmulator.IsCartridgeLoaded())
					{
						this->run = !this->run;
					}
					break;
				}
				case CustomEvents::StepEmulator:
				{
					this->frameStep = true;
					break;
				}
				case CustomEvents::IncrementDefaultColourPalette:
				{
					this->nesEmulator.IncrementDefaultColourPalette();
					break;
				}
				case CustomEvents::StepAssembly:
				{
					this->assemblyStep = true;
					break;
				}
				case CustomEvents::OpenRom:
				{
					std::string filePath = this->romLoader.GetCurrentFilesystemPath();
					std::vector<std::string> filesInPath = this->romLoader.ListFilesInPath(filePath);
					this->windowsMgr.ChangeViewType(WindowView::OpenRom);
					this->windowsMgr.GetFileNavigator()->SetFileSystemData(filePath, filesInPath);
					break;
				}
				case CustomEvents::FileSelected:
				{
					std::unique_ptr<FileNavigator>& fileNavigator = this->windowsMgr.GetFileNavigator();
					std::string file_selected = fileNavigator->GetSelectedFile();
					std::string old_file_path = this->romLoader.GetCurrentFilesystemPath();

					std::string absolutePath = FileSystem::CombinePath(old_file_path, file_selected);

					if(FileSystem::IsDir(absolutePath))
					{
						// folder
						std::string new_file_path = FileSystem::CombinePath(old_file_path, file_selected);
						this->romLoader.SetCurrentFilesystemPath(new_file_path);
						std::vector<std::string> filesInPath = this->romLoader.ListFilesInPath(new_file_path);
						fileNavigator->SetFileSystemData(new_file_path, filesInPath);
					}
					else
					{
						// file
						this->windowsMgr.ChangeViewType(WindowView::Simple);
						
						// load rom
						this->LoadMedia(absolutePath);
					}
					break;
				}
				case CustomEvents::DumpProgramRom:
				{
					int allInstructions = 100000000;
					std::string instructions = this->nesEmulator.GenerateCpuScreen(allInstructions, allInstructions);

					// write to file
					this->romLoader.WriteToFile("dumped_rom.txt", instructions);
					break;
				}
				default:
				{
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
		if(this->nesEmulator.IsCartridgeLoaded())
		{
			if(this->assemblyStep == true)
			{
				this->nesEmulator.Step(this->verbose);
			}
			else if (this->debugPoint >= 0)
			{
				const long long oldFrameCount = this->nesEmulator.GetFrameCount();
				while((oldFrameCount == this->nesEmulator.GetFrameCount()) && this->run)
				{
					this->nesEmulator.Step(this->verbose);
					int pc = this->nesEmulator.GetDma()->GetCpuMemory().GetRegisters().name.PC;
					if(pc == this->debugPoint)
					{
						this->run = false;
					}
				}
			}
			else
			{
				this->nesEmulator.processFrame(this->verbose);
			}
			this->frameStep = false;
			this->assemblyStep = false;
		}
	}
	return true;
}

bool GameEngine::Display()
{
	unsigned int fps = this->fpsTimer.CalcFps();
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

void GameEngine::SetDebugPoint(int debugLine)
{
	this->debugPoint = debugLine;
}