#ifndef MENU_BAR
#define MENU_BAR

#include <vector>

#include "IWindow.h"
#include "SubMenu.h"
#include "CustomEventMgr.h"

class MenuBar : public IWindow
{
    private:
    std::vector<SubMenu> subMenus;

    //set to true when a submenu is activated
    bool isActive = false;

    public:
    MenuBar(SDL_Renderer* gRenderer);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void AppendSubMenu(const SubMenu& button);

    void HandleEvent(const SDL_Event& e) override;
    
    void Display();
};

#endif