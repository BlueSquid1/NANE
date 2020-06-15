#ifndef BUTTON
#define BUTTON

#include <string>
#include <memory>
#include <vector>

#include "TextWindow.h"

class ToggleButton : public TextWindow
{
    private:
    std::string displayName;
    SDL_Color activeBackgroundColour;
    bool isPressed = false;
    bool enableHighlighting;
    bool isHighlighted = false;
    std::vector<SDL_Event> triggerEvents;

    public:
    ToggleButton(SDL_Renderer* gRenderer, const std::string& name, Uint32 * eventName = NULL, bool highlightOnMouse = false, SDL_Color forgroundColour = {0x00, 0x00, 0x00, 0xFF}, SDL_Color inactiveBackgroundColour = {0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color activeBackgroundColour = {0xA0, 0x40, 0x20, 0xFF}, FontType font = FontType::defaultFont);

    SDL_Rect CalculateMinSize() const;

    void RegisterEvent(Uint32 eventName);

    virtual void HandleEvent(const SDL_Event& e);

    virtual void Display();

    bool contains(int mousePosX, int mousePosY);

    //getters/setters
    bool GetIsPressed() const;
    void SetIsPressed(bool isPressed);
};

#endif