#ifndef TEXT_WINDOW
#define TEXT_WINDOW

#include "IWindow.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class TextWindow : public IWindow
{
    public:
    void Display(const std::string& screenText, TTF_Font * gFont, SDL_Color forgroundColour, SDL_Color backgroundColour, bool writeTopDown);
};

#endif