#include "PushButton.h"

#include "CustomEventMgr.h"
#include <iostream>

PushButton::PushButton(SDL_Renderer* gRenderer, const std::string& name, Uint32 * eventName, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour, FontType font)
: TextWindow(gRenderer, TextDirection::centred, forgroundColour, inactiveBackgroundColour, font),
  displayName(name),
  activeBackgroundColour(activeBackgroundColour)
{
    if(eventName != NULL)
    {
        this->RegisterEvent(*eventName);
    }
}

void PushButton::RegisterEvent(Uint32 eventName)
{
    SDL_Event triggerEvent;
    SDL_memset(&triggerEvent, 0, sizeof(triggerEvent));
    triggerEvent.type = CustomEventMgr::GetInstance()->GetCustomEventType();
    triggerEvent.user.code = eventName;
    this->triggerEvents.push_back(triggerEvent);
}

SDL_Rect PushButton::CalculateMinSize() const
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

void PushButton::HandleEvent(const SDL_Event& e)
{
    //activate or deactivate button
    if(e.button.button == SDL_BUTTON_LEFT && (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP))
    {
        if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            //check if it is in boundry
            int mousePosX = e.button.x;
            int mousePosY = e.button.y;
            if( this->contains(mousePosX, mousePosY) )
            {
                this->isPressed = true;

                if(this->isPressed)
                {
                    for(SDL_Event& event : this->triggerEvents)
                    {
                        SDL_PushEvent(&event);
                    }
                }
            }
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
            this->isPressed = false;
        }
    }

    //highlighted button on hover over if it isn't a toggle button
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

void PushButton::Display()
{
    SDL_Color * activeColour = NULL;
    if(this->isPressed || this->isHighlighted)
    {
        activeColour = & this->activeBackgroundColour;
    }
    TextWindow::Display(this->displayName, activeColour);
}

bool PushButton::contains(int mousePosX, int mousePosY) const
{
    return ( (mousePosX >= this->windowDimensions.x) && (mousePosX < (this->windowDimensions.x + this->windowDimensions.w)) )
        &&  ( (mousePosY >= this->windowDimensions.y) && (mousePosY < (this->windowDimensions.y + this->windowDimensions.h)) );
}

bool PushButton::GetIsPressed() const
{
    return this->isPressed;
}

void PushButton::SetIsPressed(bool isPressed)
{
    this->isPressed = isPressed;
}