#pragma once

#include <vector> //std::vector
#include <exception> //exceptions
#include <memory> //unique_ptr

#include "Types.h"
#include "IMemoryRW.h"

class BitUtil
{
    public:
    enum BitPosition
    {
        bit0 = 0x01,
        bit1 = 0x02,
        bit2 = 0x04,
        bit3 = 0x08,
        bit4 = 0x10,
        bit5 = 0x20,
        bit6 = 0x40,
        bit7 = 0x80
    };

    private:
    static const byte bitReverseLookup[16];
    public:
    /**
     * converts an array to a vector.
     * @param array the array to check
     * @param arrayLen the length of the array
     * @return the equivalent vector
     */
    static std::unique_ptr<std::vector<byte>> ArrayToVec(byte * array, unsigned int arrayLen);

    /**
     * tests if the content of two vectors are equal.
     * @param vec1 the first vector to compaire.
     * @param vec2 the second vector to compaire.
     * @return true if both vectors have the same content
     */
    static bool VecEqual(const std::vector<byte> * vec1, const std::vector<byte> * vec2);

    /**
     * Will split a vector and create a smaller subvector.
     * @param vec the vector to extract a subvector from.
     * @param start the start index in "vec" for the subvector (inclusive).
     * @param end the last index in "vec" for the subvector (exclusive).
     * @return the subvector.
     */
    static std::unique_ptr<std::vector<byte>> VecSplit(const std::vector<byte> * vec, unsigned int start, unsigned int end);

    /**
     * for finding values within a single byte by applying a bit mask.
     * @param value the byte to find values in
     * @param startBit the first bit in "value" you want to receive (inclusive).
     * @param endBit the last bit you want to receive (inclusive). If endBit is not specified then only the single bit specified in starting bit will be returned.
     * @return the resulting value from all the bits between startBit and endBit.
     */
    static qword GetBits(qword value, unsigned char startBit, unsigned char endBit = 0);
    
    /**
     * intepretes the 2 bytes starts at startAddress as a single dword.
     * @param memory pointer to a piece of readable memory
     * @param startAddress the first byte that makes up the dword 
     * @return the value from the memory
     */
    static dword GetDWord(IMemoryR * memory, dword startAddress, bool pageWrap = false, bool readInsteadOfSeek = false);

    /**
     * flips the bits in a byte
     * @param value the input value to flip the bits
     * @return the value flipped
     */
    static byte FlipByte(byte value);

    static float approxsin(float t);
};