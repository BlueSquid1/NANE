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
    submenuActiveBackground.r = 0x88;
    submenuActiveBackground.g = 0x22;
    submenuActiveBackground.b = 0x00;
    submenuActiveBackground.a = 0xFF;
    std::string submenuFont = "VeraMono.ttf";
    int submenuFontSize = 13;

    SubMenu fileMenu(gRenderer, "File", submenuFont, submenuFontSize, submenuForground, submenuInactiveBackground, submenuActiveBackground);
    //TODO
    //fileMenu.AddButton(Button(gRenderer, "Open"));
    this->AppendSubMenu(fileMenu);

    SubMenu viewMenu(gRenderer, "View", submenuFont, submenuFontSize, submenuForground, submenuInactiveBackground, submenuActiveBackground);
    //viewMenu.AddButton(Button(gRenderer, "Minimalistic"));
    //viewMenu.AddButton(Button(gRenderer, "Disassemble"));
    this->AppendSubMenu(viewMenu);
}

void MenuBar::SetDimensions(int posX, int posY, int width, int height)
{
    IWindow::SetDimensions(posX, posY, width, height);

    //layout the submenu buttons
    int menuSpacer = 15;

    int buttonPosX = menuSpacer;
    int buttonPosY = 0;
    for(SubMenu& subMenu : subMenus)
    {
        const SDL_Rect& minSize = subMenu.CalculateMinSize();
        subMenu.SetDimensions(buttonPosX, buttonPosY, minSize.w, height);
        buttonPosX += minSize.w;
        buttonPosX += menuSpacer;
    }
}

void MenuBar::AppendSubMenu(const SubMenu& button)
{
    this->subMenus.push_back(button);
}

void MenuBar::Display()
{
    //draw background colour
    SDL_Color backgroundColour;
    backgroundColour.r = 0x30;
    backgroundColour.g = 0x30;
    backgroundColour.b = 0x30;
    backgroundColour.a = 0xFF;
    SDL_SetRenderDrawColor( gRenderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a );
	SDL_RenderFillRect(this->gRenderer, &this->windowDimensions);

    //draw submenus
    for(SubMenu& subMenu : this->subMenus)
    {
        subMenu.Display();
    }
}