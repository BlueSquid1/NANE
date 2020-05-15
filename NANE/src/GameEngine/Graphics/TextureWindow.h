#ifndef TEXTURE_WINDOW
#define TEXTURE_WINDOW

#include "IWindow.h"
#include "NES/Memory/Matrix.h"
#include "NES/PPU/NesColour.h"

class TextureWindow : public IWindow
{
    public:
    void Display(const Matrix<rawColour>& chRomData);
};

#endif