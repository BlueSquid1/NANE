#include "IWindow.h"

void IWindow::FreeTexture()
{
	//Free texture if it exists
	if( this->windowTexture != NULL )
	{
		SDL_DestroyTexture( this->windowTexture );
		this->windowTexture = NULL;
	}
}

IWindow::~IWindow()
{
	this->FreeTexture();
}

bool IWindow::Init(SDL_Renderer * gRenderer, int width, int height, int xPos, int yPos)
{
    this->gRenderer = gRenderer;
    windowDimensions.w = width;
    windowDimensions.h = height;
    windowDimensions.x = xPos;
    windowDimensions.y = yPos;

	this->windowTexture = SDL_CreateTexture(this->gRenderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if(!this->windowTexture)
	{
		return false;
	}
	return true;
}

void IWindow::ChangeDimensions(int newWidth, int newHeight, int newX, int newY)
{
	this->windowDimensions.w = newWidth;
	this->windowDimensions.h = newHeight;
	this->windowDimensions.x = newX;
	this->windowDimensions.y = newY;
}

//getters/setters
const SDL_Rect& IWindow::GetDimensions() const
{
	this->windowDimensions;
}