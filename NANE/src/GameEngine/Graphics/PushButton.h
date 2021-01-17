#pragma once

#include <string>
#include <memory>
#include <vector>

#include "TextWindow.h"

class PushButton : public TextWindow
{
    private:
    std::string displayName;
    SDL_Color activeBackgroundColour;
    bool isPressed = false;
    bool isHighlighted = false;
    std::vector<SDL_Event> triggerEvents;

    public:
    PushButton(SDL_Renderer* gRenderer, const std::string& name, Uint32 * eventName = NULL, SDL_Color forgroundColour = {0x00, 0x00, 0x00, 0xFF}, SDL_Color inactiveBackgroundColour = {0xFF, 0xFF, 0xFF, 0xFF}, SDL_Color activeBackgroundColour = {0xA0, 0x40, 0x20, 0xFF}, FontType font = FontType::defaultFont);

    SDL_Rect CalculateMinSize() const;

    void RegisterEvent(Uint32 eventName);

    virtual void HandleEvent(const SDL_Event& e) override;

    virtual void Display();

    bool contains(int mousePosX, int mousePosY) const;

    //getters/setters
    bool GetIsPressed() const;
    void SetIsPressed(bool isPressed);
    bool GetIsHighlighted() const;
    std::string GetDisplayName() const;
};