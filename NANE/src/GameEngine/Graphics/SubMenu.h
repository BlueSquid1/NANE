#ifndef SUB_MENU
#define SUB_MENU

#include <vector>
#include <string>
#include <memory>
#include <SDL_ttf.h>
#include "Button.h"


class SubMenu : public Button
{
    private:
    std::vector<Button> menuItems;
    
    public:
    SubMenu(SDL_Renderer* gRenderer, const std::string& name, const std::string& font, int ptSize, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour);

    void AddButton(const Button& button);

    void Display() override;
};

#endif