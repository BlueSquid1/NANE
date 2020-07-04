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
    Uint32 openRomEvent = (Uint32)MenuEvents::OpenRom;
    fileMenu.AddButton(PushButton(gRenderer, "Open ROM", &openRomEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 saveSnapshotEvent = (Uint32)MenuEvents::SaveSnapshot;
    fileMenu.AddButton(PushButton(gRenderer, "Save Snapshot", &saveSnapshotEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 loadSnapshotEvent = (Uint32)MenuEvents::LoadSnapshot;
    fileMenu.AddButton(PushButton(gRenderer, "Load Snapshot", &loadSnapshotEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(fileMenu);

    SubMenu viewMenu(gRenderer, "View", submenuForground, submenuInactiveBackground, submenuActiveBackground);
    Uint32 simpleViewEvent = (Uint32)MenuEvents::SimpleView;
    viewMenu.AddButton(PushButton(gRenderer, "Simple View", &simpleViewEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 disassembleViewEvent = (Uint32)MenuEvents::DisassembleView;
    viewMenu.AddButton(PushButton(gRenderer, "Disassemble View", &disassembleViewEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 showFpsEvent = (Uint32)MenuEvents::ShowFpsCounter;
    viewMenu.AddButton(PushButton(gRenderer, "Show/Hide FPS Counter", &showFpsEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 scaleFactorEvent1 = (Uint32)MenuEvents::ScaleFactor1;
    viewMenu.AddButton(PushButton(gRenderer, "Scale 1:1", &scaleFactorEvent1, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 scaleFactorEvent2 = (Uint32)MenuEvents::ScaleFactor2;
    viewMenu.AddButton(PushButton(gRenderer, "Scale 1:2", &scaleFactorEvent2, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 scaleFactorEvent3 = (Uint32)MenuEvents::ScaleFactor3;
    viewMenu.AddButton(PushButton(gRenderer, "Scale 1:4", &scaleFactorEvent3, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 scaleFactorEvent4 = (Uint32)MenuEvents::ScaleFactor4;
    viewMenu.AddButton(PushButton(gRenderer, "Scale 1:8", &scaleFactorEvent4, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 scaleFactorEvent5 = (Uint32)MenuEvents::ScaleFactor5;
    viewMenu.AddButton(PushButton(gRenderer, "Scale 1:16", &scaleFactorEvent5, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(viewMenu);

    SubMenu emulatorMenu(gRenderer, "Emulator", submenuForground, submenuInactiveBackground, submenuActiveBackground);
    Uint32 continueEvent = (Uint32)MenuEvents::ContinuePauseEmulator;
    emulatorMenu.AddButton(PushButton(gRenderer, "Continue/Pause (p)", &continueEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 pauseEvent = (Uint32)MenuEvents::StepEmulator;
    emulatorMenu.AddButton(PushButton(gRenderer, "Step To Next Frame (n)", &pauseEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(emulatorMenu);

    SubMenu disassembleMenu(gRenderer, "Disassemble", submenuForground, submenuInactiveBackground, submenuActiveBackground);
    Uint32 IncrementDefaultColourPaletteEvent = (Uint32)MenuEvents::IncrementDefaultColourPalette;
    disassembleMenu.AddButton(PushButton(gRenderer, "Increment Default Colour Palette", &IncrementDefaultColourPaletteEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    Uint32 stepAssemblyEvent = (Uint32)MenuEvents::StepAssembly;
    disassembleMenu.AddButton(PushButton(gRenderer, "Step To Next CPU Instruction (m)", &stepAssemblyEvent, submenuForground, submenuInactiveBackground, submenuActiveBackground));
    this->AppendSubMenu(disassembleMenu);
}

void MenuBar::SetDimensions(int posX, int posY, int width, int height)
{
    IWindow::SetDimensions(posX, posY, width, height);

    //layout the submenu buttons
    int menuSpacer = 18;
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
    if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        this->isActive = false;

        int mousePosX = e.button.x;
        int mousePosY = e.button.y;

        for(SubMenu& subMenu : this->subMenus)
        {
            if(subMenu.contains(mousePosX, mousePosY))
            {
                //user selected on menubar
                this->isActive = true;
                subMenu.SetIsPressed(true);
            }
        }
    }

    // handle events raised from submenus
    for(SubMenu& subMenu : this->subMenus)
    {
        subMenu.HandleEvent(e);
    }

    if(e.type == SDL_MOUSEBUTTONUP)
    {
        int mousePosX = e.button.x;
        int mousePosY = e.button.y;

        for(SubMenu& subMenu : this->subMenus)
        {
            if(!subMenu.contains(mousePosX, mousePosY))
            {
                subMenu.SetIsPressed(false);
            }
        }
    }
    else if( e.type == SDL_MOUSEMOTION )
    {
        if(this->isActive)
        {
            int mousePosX = e.motion.x;
            int mousePosY = e.motion.y;

            //find the submenu that should be active
            int activePos = -1;
            for(int i = 0; i < this->subMenus.size(); ++i)
            {
                if(this->subMenus.at(i).contains(mousePosX, mousePosY))
                {
                    activePos = i;
                }
            }
            if(activePos >= 0)
            {
                //simulate pressing submenus under curser
                for(SubMenu& subMenu : this->subMenus)
                {
                    subMenu.SetIsPressed(false);
                }
                this->subMenus.at(activePos).SetIsPressed(true);
            }
        }
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