#include "TextureWindow.h"

void TextureWindow::FreeTexture()
{
	//Free texture if it exists
	if( this->windowTexture != NULL )
	{
		SDL_DestroyTexture( this->windowTexture );
		this->windowTexture = NULL;
	}
}

TextureWindow::TextureWindow(SDL_Renderer* gRenderer, const int textureWidth, const int textureHeight)
: IWindow(gRenderer)
{
	this->windowTexture = SDL_CreateTexture(this->gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

TextureWindow::~TextureWindow()
{
	this->FreeTexture();
}

void TextureWindow::Display(const Matrix<rawColour>& pixelData)
{
	SDL_UpdateTexture(this->windowTexture, nullptr, pixelData.dump().data(), pixelData.GetWidth() * sizeof(rawColour));
	SDL_Rect source_dimensions;
	source_dimensions.x = 0;
	source_dimensions.y = 0;
	source_dimensions.w = pixelData.GetWidth();
	source_dimensions.h = pixelData.GetHeight();
	SDL_RenderCopy(this->gRenderer, this->windowTexture, &source_dimensions, &this->windowDimensions);
}