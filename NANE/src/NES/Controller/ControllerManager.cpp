#include <iostream>
#include <exception>

#include "ControllerManager.h"

ControllerManager::ControllerManager()
: IMemoryRW(0x4016, 0x4017)
{
    this->controllers.push_back(NesController(0x4016));
    this->controllers.push_back(NesController(0x4017));
}


void ControllerManager::SetKey(NesController::NesInputs input, bool isPressed, int controllerNum)
{
    if(controllerNum < 0 || controllerNum >= this->controllers.size())
    {
        throw std::invalid_argument("invalid controller number: " + controllerNum);
    }

    this->controllers.at(controllerNum).SetKey(input, isPressed);
}

const std::vector<bool> ControllerManager::GetKeyPressed(int controllerNum) const
{
    if(controllerNum < 0 || controllerNum >= this->controllers.size())
    {
        throw std::invalid_argument("invalid controller number: " + controllerNum);
    }

    return this->controllers.at(controllerNum).GetKeyPressed();
}

void ControllerManager::Write(dword address, byte value)
{
    // write is sent to all controllers
    for(NesController& controller : this->controllers)
    {
        controller.Write(address, value);
    }
}

byte ControllerManager::Read(dword address)
{
    for(NesController& controller : this->controllers)
    {
        if(controller.Contains(address))
        {
            return controller.Read(address);
        }
    }

    std::cerr << "ControllerManager: failed to read from address: " << address;
    return 0;
}

byte ControllerManager::Seek(dword address) const
{
    for(const NesController& controller : this->controllers)
    {
        if(controller.Contains(address))
        {
            return controller.Seek(address);
        }
    }

    std::cerr << "ControllerManager: failed to seek from address: " << address;
    return 0;
}