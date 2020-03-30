#include "WindowsManager.h"

bool WindowManager::Init()
{
	std::cout << "initalizing SDL" << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cerr << "can't start SDL. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

    this->gWindow = SDL_CreateWindow("NES-NX", 0, 0, this->xPixelRes, this->yPixelRes, SDL_WINDOW_SHOWN);
	if (!this->gWindow)
	{
		std::cerr << "can't create window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	this->gRenderer = SDL_CreateRenderer(this->gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!this->gRenderer)
	{
		std::cerr << "Can't create renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	bool mainWindowResult = this->mainWindow.Init(this->gRenderer, 256, 240, 0, 0);
	if(mainWindowResult == false)
	{
		std::cerr << "can't create main window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	bool chrRomWindowResult = this->chrRomWindow.Init(this->gRenderer, 256, 128, 256, 0);
	if(chrRomWindowResult == false)
	{
		std::cerr << "can't create chr rom window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool WindowManager::Display(Nes& nesEmulator)
{
    //clear the screen
	SDL_SetRenderDrawColor(this->gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(this->gRenderer);

	//render windows
	const Matrix<rawColour>& mainDisplay = nesEmulator.GetFrameDisplay();
	mainWindow.Display(mainDisplay);

	std::unique_ptr<Matrix<rawColour>> chrRomDisplay = nesEmulator.GeneratePatternTables();
	chrRomWindow.Display(*chrRomDisplay);

	//Update the surface
	SDL_RenderPresent( this->gRenderer );
    return true;
}

void WindowManager::Close()
{
	std::cout << "Destroying SDL" << std::endl;
    SDL_DestroyWindow(this->gWindow);
	this->gWindow = NULL;
	SDL_DestroyRenderer(this->gRenderer);
	this->gRenderer = NULL;
    SDL_Quit();
}