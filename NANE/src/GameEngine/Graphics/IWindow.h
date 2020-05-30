#ifndef I_WINDOW
#define I_WINDOW

#include <SDL.h>

class IWindow
{
    protected:
    SDL_Rect windowDimensions;
    SDL_Renderer * gRenderer = NULL;

    public:
    IWindow(SDL_Renderer* gRenderer);
    virtual ~IWindow();
    void SetDimensions(int width, int height, int posX, int posY);
};

#endif