#include "Button.h"

#include <iostream>

Button::Button(SDL_Renderer* gRenderer, const std::string& name, const std::string& font, int ptSize, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour)
: TextWindow(gRenderer, font, ptSize, forgroundColour, inactiveBackgroundColour, TextDirection::middleAligned),
  displayName(name),
  activeBackgroundColour(activeBackgroundColour)
{
}

SDL_Rect Button::CalculateMinSize()
{
	SDL_Rect minSize;
	minSize.x = 0;
	minSize.y = 0;
    if(TTF_SizeText(this->font, this->displayName.c_str(), &minSize.w, &minSize.h) != 0)
    {
        std::cerr << "failed to calculate font size" << std::endl;
    }
	return minSize;
}

void Button::Display()
{
    TextWindow::Display(this->displayName);
}