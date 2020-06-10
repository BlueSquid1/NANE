#include "SubMenu.h"

#include <iostream>

SubMenu::SubMenu(SDL_Renderer* gRenderer, const std::string& name, const std::string& font, int ptSize, SDL_Color forgroundColour, SDL_Color inactiveBackgroundColour, SDL_Color activeBackgroundColour)
: Button(gRenderer, name, font, ptSize, forgroundColour, inactiveBackgroundColour, activeBackgroundColour)
{

}

void SubMenu::AddButton(const Button& button)
{
    this->menuItems.push_back(button);
}

void SubMenu::Display()
{
    Button::Display();
}