#include "CartridgeMapping0.h"

CartridgeMapping0::CartridgeMapping0()
: ICartridge(0)
{
    
}

bool CartridgeMapping0::LoadINes(std::unique_ptr<INes> INesRom)
{
    return true;
}