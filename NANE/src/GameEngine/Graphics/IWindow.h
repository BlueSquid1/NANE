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
    
    virtual void SetDimensions(int posX, int posY, int width, int height);

    const SDL_Rect& GetDimensions() const;
};

#endif