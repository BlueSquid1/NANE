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
    struct
    {
        byte lower : 8;
        byte upper : 8;
    };
};
typedef uint32_t qword; //quadtuple word
union qword_p
{
    qword val;
    struct
    {
        dword_p first;
        dword_p second;
    };
};

#endif