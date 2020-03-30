#include "Window.h"

bool Window::Init(SDL_Renderer * gRenderer, int width, int height, int xPos, int yPos)
{
    this->gRenderer = gRenderer;
    windowPosition.w = width;
    windowPosition.h = height;
    windowPosition.x = xPos;
    windowPosition.y = yPos;

	this->windowTexture = SDL_CreateTexture(this->gRenderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if(!this->windowTexture)
	{
		return false;
	}

	return true;
}

void Window::Display(const Matrix<rawColour>& pixelData)
{
	SDL_UpdateTexture(this->windowTexture, nullptr, pixelData.dump().data(), pixelData.GetWidth() * 4);
	SDL_RenderCopy(this->gRenderer, this->windowTexture, nullptr, &this->windowPosition);
}