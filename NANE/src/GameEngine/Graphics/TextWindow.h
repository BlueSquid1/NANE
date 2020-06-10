#ifndef TEXT_WINDOW
#define TEXT_WINDOW

#include "IWindow.h"

#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

enum class TextDirection
{
    topAligned,
    bottomAligned,
    middleAligned
};

class TextWindow : public IWindow
{
    protected:
    TTF_Font * font = NULL;
    SDL_Color forgroundColour;
    SDL_Color defaultBackgroundColour;
    TextDirection direction;
    
    public:
    TextWindow(SDL_Renderer * gRenderer, std::string fontFile, int fontPt, SDL_Color forgroundColour, SDL_Color backgroundColour, TextDirection direction = TextDirection::topAligned);

    void Display(const std::string& screenText, SDL_Color * overrideBackgroundColour = NULL);
};

#endif