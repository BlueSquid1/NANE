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
    ContinuePauseEmulator,
    StepEmulator,
    SaveSnapshot,
    LoadSnapshot,
    IncrementDefaultColourPalette,
    StepAssembly,
    ShowFpsCounter,
    ScaleFactor1,
    ScaleFactor2,
    ScaleFactor3,
    ScaleFactor4,
    ScaleFactor5
};

class MenuBar : public IWindow
{
    private:
    std::vector<SubMenu> subMenus;

    //set to true when a submenu is activated
    bool isActive = false;

    private:
    void RegisterEvent(PushButton& button, MenuEvents event);

    public:
    MenuBar(SDL_Renderer* gRenderer);

    void SetDimensions(int posX, int posY, int width, int height) override;

    void AppendSubMenu(const SubMenu& button);

    void HandleEvent(const SDL_Event& e);
    
    void Display();
};

#endif