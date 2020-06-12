#include "ToggleButton.h"

#include <iostream>

ToggleButton::ToggleButton(SDL_Renderer* gRenderer, const std::string& name, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour, FontType font)
: TextWindow(gRenderer, TextDirection::centred, forgroundColour, inactiveBackgroundColour, font),
  displayName(name),
  activeBackgroundColour(activeBackgroundColour)
{
}

SDL_Rect ToggleButton::CalculateMinSize() const
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

void ToggleButton::HandleEvent(const SDL_Event& e)
{
    if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
    {
        //check if it is in boundry
        int mousePosX = e.button.x;
        int mousePosY = e.button.y;
        if( ( (mousePosX >= this->windowDimensions.x) && (mousePosX <= (this->windowDimensions.x + this->windowDimensions.w)) )
        &&  ( (mousePosY >= this->windowDimensions.y) && (mousePosY <= (this->windowDimensions.y + this->windowDimensions.h)) ) )
        {
            this->isPressed = true;
        }
        else
        {
            this->isPressed = false;
        }
    }
}

void ToggleButton::Display()
{
    SDL_Color * activeColour = NULL;
    if(this->isPressed)
    {
        activeColour = & this->activeBackgroundColour;
    }
    TextWindow::Display(this->displayName, activeColour);
}

bool ToggleButton::GetIsPressed() const
{
    return this->isPressed;
}

void ToggleButton::SetIsPressed(bool isPressed)
{
    this->isPressed = isPressed;
}