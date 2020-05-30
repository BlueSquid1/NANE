#include "IWindow.h"

IWindow::IWindow(SDL_Renderer* gRenderer)
{
	this->gRenderer = gRenderer;
}

IWindow::~IWindow()
{
}

void IWindow::SetDimensions(int width, int height, int posX, int posY)
{
	this->windowDimensions.w = width;
	this->windowDimensions.h = height;
	this->windowDimensions.x = posX;
	this->windowDimensions.y = posY;
}