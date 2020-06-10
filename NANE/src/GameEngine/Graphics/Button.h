#ifndef BUTTON
#define BUTTON

#include <string>
#include <SDL_ttf.h>
#include "TextWindow.h"

class Button : public TextWindow
{
    private:
    std::string displayName;
    SDL_Color activeBackgroundColour;

    public:
    Button(SDL_Renderer* gRenderer, const std::string& name, const std::string& font, int ptSize, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour);

    SDL_Rect CalculateMinSize();

    virtual void Display();
};

#endif