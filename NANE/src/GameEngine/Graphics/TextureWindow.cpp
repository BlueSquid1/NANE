#include "TextureWindow.h"
#include <SDL.h>

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