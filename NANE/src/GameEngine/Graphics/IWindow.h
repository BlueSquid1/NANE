#ifndef I_WINDOW
#define I_WINDOW

#include <SDL.h>

class IWindow
{
    protected:
    SDL_Rect windowDimensions;
    SDL_Renderer * gRenderer = NULL;
    SDL_Texture * windowTexture = NULL;
    
    void FreeTexture();

    public:
    ~IWindow();
    bool Init(SDL_Renderer * gRenderer, int width, int height, int xPos, int yPos);
    void ChangeDimensions(int newWidth, int newHeight, int newX, int newY);

    //getters/setters
    const SDL_Rect& GetDimensions() const;
};

#endif