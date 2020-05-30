#include "MenuBar.h"

MenuBar::MenuBar(SDL_Renderer* gRenderer)
:IWindow(gRenderer)
{

}

void MenuBar::Display()
{
    SDL_Color backgroundColour;
    backgroundColour.r = 0x40;
    backgroundColour.g = 0x40;
    backgroundColour.b = 0x40;
    backgroundColour.a = 0xFF;

    SDL_SetRenderDrawColor( gRenderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a );
	SDL_RenderFillRect(this->gRenderer, &this->windowDimensions);
}