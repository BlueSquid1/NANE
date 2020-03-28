#include "Screen.h"

int Screen::Flatten(int y, int x) const
{
    if(y < 0 || y >= this->maxHeight)
    {
        return -1;
    }
    else if(x < 0 || x >= this->maxWidth)
    {
        return -1;
    }
    
    return (y * this->maxWidth) + x;
}

Screen::Screen(int maxWidth, int maxHeight)
: framebuffer(maxWidth * maxHeight)
{
    this->maxHeight = maxHeight;
    this->maxWidth = maxWidth;
}

const std::vector<rawColour>& Screen::dump() const
{
    return this->framebuffer;
}
rawColour Screen::Get(int y, int x) const
{
    int index = this->Flatten(y, x);
    if(index == -1)
    {
        rawColour blank = {0};
        return blank;
    }

    return this->framebuffer.at(index);
}

void Screen::Set(int y, int x, rawColour value)
{
    int index = this->Flatten(y, x);
    if(index == -1)
    {
        return;
    }

    this->framebuffer.at(index) = value;
}