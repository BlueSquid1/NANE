#include "TextWindow.h"

#include <algorithm>

TextWindow::TextWindow(SDL_Renderer * gRenderer, TextDirection direction, SDL_Color forgroundColour, SDL_Color backgroundColour, FontType font)
 : IWindow(gRenderer)
{
	this->font = FontManager::GetInstance()->LoadFont(font);
	this->forgroundColour = forgroundColour;
	this->defaultBackgroundColour = backgroundColour;
	this->direction = direction;
}

void TextWindow::Display(const std::string& screenText, SDL_Color * overrideBackgroundColour)
{
	SDL_Color * selectedBackground = &this->defaultBackgroundColour;

	if(overrideBackgroundColour != NULL)
	{
		selectedBackground = overrideBackgroundColour;
	}
	//colour in the background
	if(selectedBackground->a != 0x00)
	{
		SDL_SetRenderDrawColor( gRenderer, selectedBackground->r, selectedBackground->g, selectedBackground->b, selectedBackground->a );
		SDL_RenderFillRect(this->gRenderer, &this->windowDimensions);
	}

	//make foreground
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped( font, screenText.c_str(), forgroundColour, 1000);
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	//Create texture from surface pixels
    SDL_Texture * windowTexture = SDL_CreateTextureFromSurface( this->gRenderer, textSurface );
	if( windowTexture == NULL )
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
	sourceSize.y = 0;
	sourceSize.w = std::min(textWidth, this->windowDimensions.w);
	sourceSize.h = std::min(textHeight, this->windowDimensions.h);

	SDL_Rect targetSize;
	switch(this->direction)
	{
		case TextDirection::topLeft:
		{
			targetSize.x = this->windowDimensions.x;
			targetSize.y = this->windowDimensions.y;
			break;
		}
		case TextDirection::bottomLeft:
		{
			targetSize.x = this->windowDimensions.x;
			targetSize.y = this->windowDimensions.y + std::max(this->windowDimensions.h - textHeight, 0);
			break;
		}
		case TextDirection::centred:
		{
			targetSize.x = this->windowDimensions.x + (std::max(this->windowDimensions.x - textWidth, 0) / 2);
			targetSize.y = this->windowDimensions.y + (std::max(this->windowDimensions.h - textHeight, 0) / 2);
			break;
		}
	}
	targetSize.w = sourceSize.w;
	targetSize.h = sourceSize.h;

	//render
	SDL_RenderCopy(this->gRenderer, windowTexture, &sourceSize, &targetSize);

	SDL_DestroyTexture( windowTexture );
}