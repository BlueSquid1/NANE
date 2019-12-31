#ifndef MAPPING_0
#define MAPPING_0

#include <memory> //std::unique_ptr

#include "ICartridge.h"
#include "../INes.h"

class CartridgeMapping0 : public ICartridge
{
    public:
    //constructor
    CartridgeMapping0();

    virtual bool LoadINes(std::unique_ptr<INes> INesRom);
};

#endif