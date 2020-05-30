#ifndef TEXT_WINDOW
#define TEXT_WINDOW

#include "IWindow.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class TextWindow : public IWindow
{
    private:
    TTF_Font * font = NULL;
    SDL_Color forgroundColour;
    SDL_Color backgroundColour;
    bool writeTopDown;
    
    public:
    TextWindow(SDL_Renderer * gRenderer, std::string fontFile, int fontPt, SDL_Color forgroundColour, SDL_Color backgroundColour, bool writeTopDown = true);
    ~TextWindow() override;
    void Display(const std::string& screenText);
};

#endif