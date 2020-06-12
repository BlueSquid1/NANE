#ifndef TEXT_WINDOW
#define TEXT_WINDOW

#include <memory>
#include <SDL.h>
#include <string>

#include "IWindow.h"
#include "FontManager.h"

enum class TextDirection
{
    topLeft,
    bottomLeft,
    centred
};

class TextWindow : public IWindow
{
    protected:
    TTF_Font * font = NULL;
    SDL_Color forgroundColour;
    SDL_Color defaultBackgroundColour;
    TextDirection direction;
    
    public:
    TextWindow(SDL_Renderer * gRenderer, TextDirection direction = TextDirection::topLeft, SDL_Color forgroundColour = {0x00, 0x00, 0x00, 0xFF}, SDL_Color backgroundColour = {0xFF, 0xFF, 0xFF, 0xFF}, FontType font = FontType::defaultFont);

    void Display(const std::string& screenText, SDL_Color * overrideBackgroundColour = NULL);
};

#endif