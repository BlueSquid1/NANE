#pragma once

#include <memory> //std::unique_ptr

#include "ICartridge.h"
#include "../INes.h"

class CartridgeMapping0 : public ICartridge
{
    public:
    //constructor
    CartridgeMapping0();

    bool LoadINes(std::shared_ptr<INes> INesRom) override;
};