#ifndef MENU_BAR
#define MENU_BAR

#include <vector>
#include "IWindow.h"
#include "SubMenu.h"

class MenuBar : public IWindow
{
    private:
    std::vector<SubMenu> subMenus;

    public:
    MenuBar(SDL_Renderer* gRenderer);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void AppendSubMenu(const SubMenu& button);

    void handleEvents();
    
    void Display();
};

#endif