#ifndef WINDOW
#define WINDOW

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <sstream>
#include "NES/PPU/Matrix.h"
#include "NES/PPU/NesColour.h"

class Window
{
    SDL_Rect windowPosition;
    SDL_Texture * windowTexture = NULL;
    SDL_Renderer * gRenderer = NULL;

    template <typename Out>
    void split(const std::string &s, char delim, Out result) {
        std::istringstream iss(s);
        std::string item;
        while (std::getline(iss, item, delim)) {
            *result++ = item;
        }
    }
    std::vector<std::string> split(const std::string &s, char delim);
    
    void FreeTexture();

    public:
    ~Window();
    bool Init(SDL_Renderer * gRenderer, int width, int height, int xPos, int yPos);
    void Display(const Matrix<rawColour>& chRomData);
    void Display(std::string& screenText, TTF_Font * gFont);
};

#endif