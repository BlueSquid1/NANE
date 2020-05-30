#ifndef MENU_BAR
#define MENU_BAR

#include <vector>
#include "IWindow.h"
#include "MenuButton.h"

class MenuBar : public IWindow
{
    private:
    std::vector<MenuButton> menuButtons;
    public:
    MenuBar(SDL_Renderer* gRenderer);

    void AppendButton(const MenuButton& button);

    void handle_events();
    
    void Display();
};

#endif