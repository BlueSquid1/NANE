#include "GraphicsEngine.h"

bool GraphicsEngine::Init()
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

	this->nesTexture = SDL_CreateTexture(this->gRenderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
	if(!this->nesTexture)
	{
		std::cerr << "can't create NES texture for rendering. SDL error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool GraphicsEngine::Display(std::unique_ptr<Matrix<rawColour>>& nesFrame)
{
    //clear the screen
	SDL_SetRenderDrawColor(this->gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(this->gRenderer);

	//render graphics
	SDL_UpdateTexture(this->nesTexture, nullptr, nesFrame->dump().data(), 256 * 4);
	SDL_Rect screen_rect;
	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.w = 256 * 3;
	screen_rect.h = 240 * 3;
	SDL_RenderCopy(this->gRenderer, this->nesTexture, nullptr, &screen_rect);

	//SDL_SetRenderDrawColor(this->gRenderer, 0xFF, 0x00, 0x00, 0xFF);
    //SDL_Rect r = {500, 500, 64, 64};
    //SDL_RenderFillRect(this->gRenderer, &r);

	//Update the surface
	SDL_RenderPresent( this->gRenderer );
    return true;
}

void GraphicsEngine::Close()
{
	std::cout << "Destroying SDL" << std::endl;
    SDL_DestroyWindow(this->gWindow);
	this->gWindow = NULL;
	SDL_DestroyRenderer(this->gRenderer);
	this->gRenderer = NULL;
    SDL_Quit();
}