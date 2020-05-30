#include "TextWindow.h"
#include <algorithm>

TextWindow::TextWindow(SDL_Renderer * gRenderer, std::string fontFile, int fontPt, SDL_Color forgroundColour, SDL_Color backgroundColour, bool writeTopDown)
 : IWindow(gRenderer)
{
	this->font = TTF_OpenFont( fontFile.c_str(), fontPt );
	this->forgroundColour = forgroundColour;
	this->backgroundColour = backgroundColour;
	this->writeTopDown = writeTopDown;
}

TextWindow::~TextWindow()
{
	if(this->font != NULL)
	{
		TTF_CloseFont(this->font);
		this->font = NULL;
	}
}

void TextWindow::Display(const std::string& screenText)
{
	//colour in the background
	if(backgroundColour.a != 0x00)
	{
		SDL_SetRenderDrawColor( gRenderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a );
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
	SDL_RenderCopy(this->gRenderer, windowTexture, &sourceSize, &targetSize);

	SDL_DestroyTexture( windowTexture );
}