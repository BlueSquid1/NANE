#include "Window.h"

std::vector<std::string> Window::split(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void Window::FreeTexture()
{
	//Free texture if it exists
	if( this->windowTexture != NULL )
	{
		SDL_DestroyTexture( this->windowTexture );
		this->windowTexture = NULL;
	}
}

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

void Window::Display(std::string& screenText, TTF_Font * gFont)
{
	//std::vector<std::string> textLines = this->split(screenText, '\n');

	//std::string text = textLines.at(0) + "hello";

	this->FreeTexture();
	SDL_Color blackColour = {0xFF, 0xFF, 0xFF};
	//SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, text.c_str(), blackColour );
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped( gFont, screenText.c_str(), blackColour, 1000);
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	//Create texture from surface pixels
    this->windowTexture = SDL_CreateTextureFromSurface( this->gRenderer, textSurface );
	if( this->windowTexture == NULL )
	{
		printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
	}
	SDL_FreeSurface( textSurface );
	SDL_RenderCopy(this->gRenderer, this->windowTexture, nullptr, &this->windowPosition);
}

Window::~Window()
{
	this->FreeTexture();
}