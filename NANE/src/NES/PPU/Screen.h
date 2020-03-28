#ifndef SCREEN
#define SCREEN

#include <vector>

#include "NesColour.h"

class Screen
{
    int maxWidth;
    int maxHeight;
    std::vector<rawColour> framebuffer;

    int Flatten(int y, int x) const;
    public:
    Screen(int maxWidth, int maxHeight);

    const std::vector<rawColour>& dump() const;
    rawColour Get(int y, int x) const;
    void Set(int y, int x, rawColour value);
};

#endif