#ifndef CONTROLLER_MANAGER
#define CONTROLLER_MANAGER

#include <vector>

#include "NesController.h"
#include "NES/Memory/IMemoryRW.h"

class ControllerManager : public IMemoryRW
{
    private:
    std::vector<NesController> controllers;

    public:
    ControllerManager();

    void SetKey(NesController::NesInputs input, bool isPressed, int controllerNum);
    const std::vector<bool> GetKeyPressed(int controllerNum) const;
    
    void Write(dword address, byte value) override;
    byte Read(dword address) override;
    byte Seek(dword address) const override;
};

#endif