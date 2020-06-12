#ifndef BUTTON
#define BUTTON

#include <string>
#include "TextWindow.h"

class ToggleButton : public TextWindow
{
    private:
    std::string displayName;
    SDL_Color activeBackgroundColour;
    bool isPressed = false;

    public:
    ToggleButton(SDL_Renderer* gRenderer, const std::string& name, SDL_Color forgroundColour = {0x00, 0x00, 0x00, 0xFF}, SDL_Color inactiveBackgroundColour = {0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color activeBackgroundColour = {0xA0, 0x40, 0x20, 0xFF}, FontType font = FontType::defaultFont);

    SDL_Rect CalculateMinSize() const;

    void HandleEvent(const SDL_Event& e);

    virtual void Display();

    //getters/setters
    bool GetIsPressed() const;
    void SetIsPressed(bool isPressed);
};

#endif