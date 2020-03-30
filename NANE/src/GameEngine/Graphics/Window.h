#ifndef WINDOW
#define WINDOW

#include <SDL.h>
#include "NES/PPU/Matrix.h"
#include "NES/PPU/NesColour.h"

class Window
{
    SDL_Rect windowPosition;
    SDL_Texture * windowTexture = NULL;
    SDL_Renderer * gRenderer = NULL;

    public:
    bool Init(SDL_Renderer * gRenderer, int width, int height, int xPos, int yPos);
    void Display(const Matrix<rawColour>& chRomData);
};

#endif