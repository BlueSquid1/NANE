#ifndef MENU_BUTTON
#define MENU_BUTTON

#include <vector>
#include "IWindow.h"
#include "Button.h"

class MenuButton : public IWindow
{
    private:
    std::vector<Button> buttons;
    
    public:
    void handle_events();
};

#endif