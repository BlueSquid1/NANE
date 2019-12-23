#include "GraphicsEngine.h"

bool GraphicsEngine::Init()
{
	std::cout << "initalizing SDL." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cout << "can't start SDL. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

    this->gWindow = SDL_CreateWindow("NES-NX", 0, 0, this->xPixelRes, this->yPixelRes, SDL_WINDOW_SHOWN);
	if (!gWindow)
	{
		std::cout << "can't create window. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

    /*
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!gRenderer)
	{
		std::cout << "Can't create renderer. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}
    */
   this->gSurface = SDL_GetWindowSurface(this->gWindow);
	return true;
}

bool GraphicsEngine::Display()
{
    //Fill the surface white
	SDL_FillRect( this->gSurface, NULL, SDL_MapRGB( this->gSurface->format, 0x00, 0xFF, 0x00 ) );
			
	//Update the surface
	SDL_UpdateWindowSurface( this->gWindow );
    return true;
}

void GraphicsEngine::Close()
{
	std::cout << "Destroying SDL." << std::endl;
    SDL_DestroyWindow(this->gWindow);
	this->gWindow = NULL;
    SDL_Quit();
}