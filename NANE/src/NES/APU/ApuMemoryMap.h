#pragma once

#include "NES/Memory/BitUtil.h"
#include "NES/Memory/IMemoryRW.h"

#include "ApuRegisters.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "NoiseWave.h"

class ApuMemoryMap : public IMemoryRW
{
    private:

    //TODO 
    //1786800
    //1789773
    const int cpuClockRateHz = 1789773;
    const int frameCounterRateHz = 240;

    bool resetFrameCounter = false;

    ApuRegisters apuRegMem;

    SquareWave sq1;
    SquareWave sq2;

    TriangleWave tri;

    NoiseWave noise;

    long long totalApuCycles = 0;

    public:
    //constructor
    ApuMemoryMap();
    bool PowerCycle();
    byte Read(dword address) override;
    void Write(dword address, byte value) override;
    byte Seek(dword address) const override;
    bool Contains(dword address) const override;

    //getters and setters
    int GetCpuClockRateHz();
    int GetFrameCounterRateHz();
    const long long& GetTotalApuCycles() const;
    void SetTotalApuCycles(const long long& value);
    ApuRegisters& GetRegisters();
    SquareWave& GetSquareWave1();
    SquareWave& GetSquareWave2();
    TriangleWave& GetTriangleWave();
    NoiseWave& GetNoiseWave();
    bool GetResetFrameCounter();
    void SetResetFrameCounter( bool frameCounterReset );
};