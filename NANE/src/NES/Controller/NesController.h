#ifndef NES_CONTROLLER
#define NES_CONTROLLER

#include <vector>

#include <NES/Memory/IMemoryRW.h>

class NesController : public IMemoryRW
{
    public:
    enum NesInputs
    {
        UNDEFINED = -1,
        A = 0,
        B = 1,
        SELECT = 2,
        START = 3,
        UP = 4,
        DOWN = 5,
        LEFT = 6,
        RIGHT = 7
    };

    private:
    std::vector<bool> keyPressed;
    int bufferIndex = 8;
    std::vector<bool> readBuffer;

    public:
    NesController(dword cpuAddress);
    void SetKey(NesInputs& input, bool isPressed);

    void Write(dword address, byte value) override;
    byte Read(dword address) override;
    byte Seek(dword address) const override;

    //getter/setters
    const std::vector<bool>& GetKeyPressed() const;
};

#endif