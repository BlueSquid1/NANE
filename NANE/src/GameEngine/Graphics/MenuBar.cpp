#include "MenuBar.h"

MenuBar::MenuBar(SDL_Renderer* gRenderer)
:IWindow(gRenderer)
{
    //submenu colours
    SDL_Color submenuForground;
    submenuForground.r = 0xFF;
    submenuForground.g = 0xFF;
    submenuForground.b = 0xFF;
    submenuForground.a = 0xFF;
    SDL_Color submenuInactiveBackground;
    submenuInactiveBackground.a = 0x00;
    SDL_Color submenuActiveBackground;
    submenuActiveBackground.r = 0xAA;
    submenuActiveBackground.g = 0x22;
    submenuActiveBackground.b = 0x00;
    submenuActiveBackground.a = 0xFF;

    SubMenu fileMenu(gRenderer, "File", submenuForground, submenuInactiveBackground, submenuActiveBackground);
    fileMenu.AddButton(ToggleButton(gRenderer, "Open ROM", submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(fileMenu);

    SubMenu viewMenu(gRenderer, "View", submenuForground, submenuInactiveBackground, submenuActiveBackground);
    viewMenu.AddButton(ToggleButton(gRenderer, "Simple", submenuForground, submenuInactiveBackground, submenuActiveBackground));
    viewMenu.AddButton(ToggleButton(gRenderer, "Disassemble", submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(viewMenu);

    SubMenu emulatorView(gRenderer, "Emulator", submenuForground, submenuInactiveBackground, submenuActiveBackground);
    emulatorView.AddButton(ToggleButton(gRenderer, "Pause", submenuForground, submenuInactiveBackground, submenuActiveBackground));
    emulatorView.AddButton(ToggleButton(gRenderer, "Start", submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(emulatorView);
}

void MenuBar::SetDimensions(int posX, int posY, int width, int height)
{
    IWindow::SetDimensions(posX, posY, width, height);

    //layout the submenu buttons
    int menuSpacer = 8;
    int buttonPosX = menuSpacer;
    int buttonPosY = 0;
    for(SubMenu& subMenu : subMenus)
    {
        const SDL_Rect& minSize = subMenu.CalculateMinSize();
        int buttonWidth = minSize.w + menuSpacer;
        subMenu.SetDimensions(buttonPosX, buttonPosY, buttonWidth, height);
        buttonPosX += buttonWidth;
    }
}

void MenuBar::AppendSubMenu(const SubMenu& button)
{
    this->subMenus.push_back(button);
}

void MenuBar::HandleEvent(const SDL_Event& e)
{
    for(SubMenu& subMenu : this->subMenus)
    {
        subMenu.HandleEvent(e);
    }
}

void MenuBar::Display()
{
    //draw background colour
    SDL_Color backgroundColour;
    backgroundColour.r = 0x35;
    backgroundColour.g = 0x35;
    backgroundColour.b = 0x35;
    backgroundColour.a = 0xFF;
    SDL_SetRenderDrawColor( gRenderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a );
	SDL_RenderFillRect(this->gRenderer, &this->windowDimensions);

    //draw submenus
    for(SubMenu& subMenu : this->subMenus)
    {
        subMenu.Display();
    }
}