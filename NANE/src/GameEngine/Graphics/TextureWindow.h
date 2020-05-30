#ifndef TEXTURE_WINDOW
#define TEXTURE_WINDOW

#include "IWindow.h"
#include "NES/Memory/Matrix.h"
#include "NES/PPU/NesColour.h"

class TextureWindow : public IWindow
{
    private:
    SDL_Texture * windowTexture = NULL;

    void FreeTexture();
    
    public:
    TextureWindow(SDL_Renderer* gRenderer, const int textureWidth, const int textureHeight);
    ~TextureWindow() override;
    void Display(const Matrix<rawColour>& chRomData);
};

#endif