#pragma once

#include <vector>
#include <string>
#include <memory>
#include "PushButton.h"


class SubMenu : public PushButton
{
    private:
    SDL_Rect dropdownMenuDimensions;
    std::vector<PushButton> menuItems;
    
    public:
    SubMenu(SDL_Renderer* gRenderer, const std::string& name, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour, FontType font = FontType::defaultFont);

    void AddButton(const PushButton& button);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void HandleEvent(const SDL_Event& e) override;

    void Display() override;
};