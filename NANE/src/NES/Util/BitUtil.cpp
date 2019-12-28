#include "BitUtil.h"

std::unique_ptr<std::vector<byte>> BitUtil::ArrayToVec(byte * array, unsigned int arrayLen)
{
    std::unique_ptr<std::vector<byte>> returnVec( new std::vector<byte>(arrayLen) );
    for(int i = 0; i < arrayLen; ++i)
    {;
        returnVec->at(i) = array[i];
    }
    return returnVec;
}

bool BitUtil::VecEqual(std::vector<byte> * vec1, std::vector<byte> * vec2)
{
    if( vec1->size() != vec2->size() )
    {
        return false;
    }

    for(int i = 0; i < vec1->size(); ++i)
    {
        if(vec1->at(i) != vec2->at(i))
        {
            return false;
        }
    }
    return true;
}

std::unique_ptr<std::vector<byte>> BitUtil::VecSplit(std::vector<byte> * vec, unsigned int start, unsigned int end)
{
    if( start > end )
    {
        throw std::invalid_argument("start is greater than end");
    }

    if( end >= vec->size() )
    {
        throw std::invalid_argument("end is greater than vector length");
    }

    int len = end - start;
    std::unique_ptr<std::vector<byte>> returnVec(new std::vector<byte>(len));
    for(int i = 0; i < len; ++i)
    {
        returnVec->at(i) = vec->at(i + start);
    }
    return returnVec;
}

unsigned char BitUtil::GetBits(byte value, unsigned char startBit, unsigned char endBit)
{
    byte startMask = 0x1;
    byte mask = startMask << startBit;
    for(int i = startBit+1; i <= endBit; ++i)
    {
        mask = mask | startMask << i;
    }
    return value & mask;
}