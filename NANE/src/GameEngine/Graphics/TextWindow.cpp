#include "TextWindow.h"
#include <algorithm>


void TextWindow::Display(const std::string& screenText, TTF_Font * gFont, SDL_Color forgroundColour, SDL_Color backgroundColour, bool writeTopDown)
{
	//delete previous texture
	this->FreeTexture();

	//colour in the background
	SDL_SetRenderDrawColor( gRenderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, 0xFF );
	SDL_RenderFillRect(this->gRenderer, &this->windowDimensions);

	//make foreground
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped( gFont, screenText.c_str(), forgroundColour, 1000);
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
	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	SDL_FreeSurface( textSurface );
	textSurface = NULL;

	//scale text for display
	SDL_Rect sourceSize;
	sourceSize.x = 0;
	sourceSize.y = std::max(textHeight - this->windowDimensions.h, 0);
	sourceSize.w = std::min(textWidth, this->windowDimensions.w);
	sourceSize.h = std::min(textHeight, this->windowDimensions.h);

	SDL_Rect targetSize;
	targetSize.x = this->windowDimensions.x;
	targetSize.y = this->windowDimensions.y;
	if(writeTopDown == false)
	{
		targetSize.y = this->windowDimensions.y + std::max(this->windowDimensions.h - textHeight, 0);
	}
	targetSize.w = sourceSize.w;
	targetSize.h = sourceSize.h;

	//render
	SDL_RenderCopy(this->gRenderer, this->windowTexture, &sourceSize, &targetSize);
}