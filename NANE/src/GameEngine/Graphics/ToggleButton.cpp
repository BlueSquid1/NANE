#include "ToggleButton.h"

#include "CustomEventMgr.h"
#include <iostream>

ToggleButton::ToggleButton(SDL_Renderer* gRenderer, const std::string& name, Uint32 * eventName, bool highlightOnMouse, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour, FontType font)
: TextWindow(gRenderer, TextDirection::centred, forgroundColour, inactiveBackgroundColour, font),
  displayName(name),
  activeBackgroundColour(activeBackgroundColour),
  enableHighlighting(highlightOnMouse)
{
    if(eventName != NULL)
    {
        this->RegisterEvent(*eventName);
    }
}

void ToggleButton::RegisterEvent(Uint32 eventName)
{
    SDL_Event triggerEvent;
    SDL_memset(&triggerEvent, 0, sizeof(triggerEvent));
    triggerEvent.type = CustomEventMgr::GetInstance()->GetCustomEventType();
    triggerEvent.user.code = eventName;
    this->triggerEvents.push_back(triggerEvent);
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
    //activate or deactivate button
    if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
    {
        //check if it is in boundry
        int mousePosX = e.button.x;
        int mousePosY = e.button.y;
        if( this->contains(mousePosX, mousePosY) )
        {
            this->isPressed = true;

            for(SDL_Event& event : this->triggerEvents)
            {
                SDL_PushEvent(&event);
            }
        }
        else
        {
            this->isPressed = false;
        }
    }

    //highlighted or not highlighted
    if(this->enableHighlighting)
    {
        if(e.type == SDL_MOUSEMOTION)
        {
            int mousePosX = e.motion.x;
            int mousePosY = e.motion.y;

            if(this->contains(mousePosX, mousePosY))
            {
                this->isHighlighted = true;
            }
            else
            {
                this->isHighlighted = false;
            }
        }
    }
}

void ToggleButton::Display()
{
    SDL_Color * activeColour = NULL;
    if(this->isPressed || this->isHighlighted)
    {
        activeColour = & this->activeBackgroundColour;
    }
    TextWindow::Display(this->displayName, activeColour);
}

bool ToggleButton::contains(int mousePosX, int mousePosY)
{
    return ( (mousePosX >= this->windowDimensions.x) && (mousePosX <= (this->windowDimensions.x + this->windowDimensions.w)) )
        &&  ( (mousePosY >= this->windowDimensions.y) && (mousePosY <= (this->windowDimensions.y + this->windowDimensions.h)) );
}

bool ToggleButton::GetIsPressed() const
{
    return this->isPressed;
}

void ToggleButton::SetIsPressed(bool isPressed)
{
    this->isPressed = isPressed;
}