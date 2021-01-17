#pragma once

#include <vector>
#include <iostream>

/**
 * just a standard matrix implementation using a flat vector as the underlying vecvecData structure.
 */
template <class T>
class Matrix
{
    private:
    int maxWidth;
    int maxHeight;
    std::vector<T> vecData;

    void SetDimensions(int maxWidth, int maxHeight)
    {
        this->maxHeight = maxHeight;
        this->maxWidth = maxWidth;
    }

    int Flatten(int y, int x) const
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

    public:
    Matrix(int maxWidth, int maxHeight)
    : vecData(maxWidth * maxHeight)
    {
        this->SetDimensions(maxWidth, maxHeight);
    }

    Matrix(int maxWidth, int maxHeight, const T& defaultValue)
    : vecData(maxWidth * maxHeight, defaultValue)
    {
        this->SetDimensions(maxWidth, maxHeight);
    }

    const std::vector<T>& dump() const
    {
        return this->vecData;
    }

    T& Get(int y, int x)
    {
        int index = this->Flatten(y, x);
        if(index == -1)
        {
            throw std::invalid_argument("try to read outside the boundary for a matrix");
        }

        return this->vecData.at(index);
    }

    void Set(int y, int x, T& value)
    {
        int index = this->Flatten(y, x);
        if(index == -1)
        {
            throw std::invalid_argument("try to write outside the boundary for a matrix");
        }

        this->vecData.at(index) = value;
    }

    void SetRegion(int y, int x, int width, int height, T& value)
    {
        if(width <= 0 || height <= 0)
        {
            throw std::invalid_argument("height and width must be greater than 0 for a matrix SetRegion.");
        }
        for(int yNum = 0; yNum < height; ++yNum)
        {
            for(int xNum = 0; xNum < width; ++xNum)
            {
                this->Set(y + yNum, x + xNum, value);
            }
        }
    }

    //getters/setters
    int GetWidth() const
    {
        return this->maxWidth;
    }
    int GetHeight() const
    {
        return this->maxHeight;
    }
};
