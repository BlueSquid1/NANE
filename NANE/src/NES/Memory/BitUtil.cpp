#include "BitUtil.h"

const byte BitUtil::bitReverseLookup[16] = {
        0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
        0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
    };

std::unique_ptr<std::vector<byte>> BitUtil::ArrayToVec(byte * array, unsigned int arrayLen)
{
    if(array == NULL)
    {
        return NULL;
    }
    std::unique_ptr<std::vector<byte>> returnVec( new std::vector<byte>(arrayLen) );
    for(int i = 0; i < arrayLen; ++i)
    {;
        returnVec->at(i) = array[i];
    }
    return returnVec;
}

bool BitUtil::VecEqual(const std::vector<byte> * vec1, const std::vector<byte> * vec2)
{
    if(vec1 == NULL)
    {
        if(vec2 == NULL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
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

std::unique_ptr<std::vector<byte>> BitUtil::VecSplit(const std::vector<byte> * vec, unsigned int start, unsigned int end)
{
    if( start >= end )
    {
        throw std::invalid_argument("start is greater than or equal to end");
    }

    if( end > vec->size() )
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

qword BitUtil::GetBits(qword value, unsigned char startBit, unsigned char endBit)
{
    qword startMask = 0x1;
    qword mask = startMask << startBit;
    for(int i = startBit+1; i <= endBit; ++i)
    {
        mask = mask | startMask << i;
    }
    return (value & mask) >> startBit;
}


dword BitUtil::GetDWord(IMemoryR * memory, dword startAddress, bool pageWrap, bool readInsteadOfSeek)
{
    byte lowerByte = 0;
    if(readInsteadOfSeek == true)
    {
        lowerByte = memory->Read(startAddress);
    }
    else
    {
        lowerByte = memory->Seek(startAddress);
    }
    
    dword endAddress = startAddress + 1;
    if(pageWrap == true)
    {
        //wrap around page
        dword offset = endAddress % 256;
        dword basePage = startAddress & 0xFF00;
        endAddress = basePage + offset;
    }
    byte upperByte = 0;
    if(readInsteadOfSeek == true)
    {
        upperByte = memory->Read(endAddress);
    }
    else
    {
        upperByte = memory->Seek(endAddress);
    }
    return (upperByte << 8) | lowerByte;
}

byte BitUtil::FlipByte(byte value)
{
    return (bitReverseLookup[value & 0b1111] << 4) | bitReverseLookup[value >> 4];
}