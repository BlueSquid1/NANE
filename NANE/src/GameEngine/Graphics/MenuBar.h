#ifndef MENU_BAR
#define MENU_BAR

#include <vector>
#include "IWindow.h"
#include "SubMenu.h"

enum class MenuEvents : Uint32
{
    OpenRom,
    SimpleView,
    DisassembleView,
    PauseEmulator,
    ContinueEmulator
};

class MenuBar : public IWindow
{
    private:
    std::vector<SubMenu> subMenus;

    private:
    void RegisterEvent(ToggleButton& button, MenuEvents event);

    public:
    MenuBar(SDL_Renderer* gRenderer);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void AppendSubMenu(const SubMenu& button);

    void HandleEvent(const SDL_Event& e);
    
    void Display();
};

#endif