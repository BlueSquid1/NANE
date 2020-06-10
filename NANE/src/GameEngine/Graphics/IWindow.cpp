#include "IWindow.h"

IWindow::IWindow(SDL_Renderer* gRenderer)
{
	this->gRenderer = gRenderer;
}

IWindow::~IWindow()
{
}

void IWindow::SetDimensions(int posX, int posY, int width, int height)
{
	this->windowDimensions.w = width;
	this->windowDimensions.h = height;
	this->windowDimensions.x = posX;
	this->windowDimensions.y = posY;
}

const SDL_Rect& IWindow::GetDimensions() const
{
	return this->windowDimensions;
}