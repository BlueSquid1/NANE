#include "NesController.h"

NesController::NesController(dword cpuAddress)
: IMemoryRW(cpuAddress, cpuAddress), 
  keyPressed(8, false),
  readBuffer(8, false)
{

}

void NesController::SetKey(NesInputs& input, bool isPressed)
{
    if(input == NesInputs::UNDEFINED)
    {
        return;
    }
    
    this->keyPressed.at(input) = isPressed;
}

void NesController::Write(dword address, byte value)
{
    for(int i = 0; i < 8; ++i)
    {
        this->readBuffer.at(i) = this->keyPressed.at(i);
    }
    this->bufferIndex = 0;
}

byte NesController::Read(dword address)
{
    if(this->bufferIndex >= 8)
    {
        return true;
    }
    int keyPress = this->readBuffer.at(this->bufferIndex);
    ++this->bufferIndex;
    return keyPress;
}

byte NesController::Seek(dword address) const
{
    // official Nintendo brand controllers always return 1 by default
    return true;
}

const std::vector<bool>& NesController::GetKeyPressed() const
{
    return this->keyPressed;
}