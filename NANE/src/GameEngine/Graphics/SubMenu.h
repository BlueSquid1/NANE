#ifndef SUB_MENU
#define SUB_MENU

#include <vector>
#include <string>
#include <memory>
#include "ToggleButton.h"


class SubMenu : public ToggleButton
{
    private:
    SDL_Rect dropdownMenuDimensions;
    std::vector<ToggleButton> menuItems;
    
    public:
    SubMenu(SDL_Renderer* gRenderer, const std::string& name, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour, FontType font = FontType::defaultFont);

    void AddButton(const ToggleButton& button);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void Display() override;
};

#endif