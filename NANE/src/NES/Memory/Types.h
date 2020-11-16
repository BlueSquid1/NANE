#ifndef TYPES
#define TYPES

#include <cstdint>

typedef bool bit;
typedef uint8_t byte;
typedef int8_t sByte;
typedef uint16_t dword; //double word
union dword_p
{
    dword val;
    #pragma pack(push, 1)
    struct
    {
        byte lower;
        byte upper;
    };
    #pragma pack(pop)
};
typedef uint32_t qword; //quadtuple word
union qword_p
{
    qword val;
    #pragma pack(push, 1)
    struct
    {
        dword_p first;
        dword_p second;
    };
    #pragma pack(pop)
};

#endif