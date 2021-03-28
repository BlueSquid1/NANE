#include "ApuMemoryMap.h"

namespace
{
    const int JOYSTICK_STROBE_ADDRESS = 0x4016;
}

ApuMemoryMap::ApuMemoryMap()
: IMemoryRW(0x4000, 0x4017),
  sq1(false),
  sq2(true)
{
}

bool ApuMemoryMap::PowerCycle()
{
    bool regReturn = this->GetRegisters().PowerCycle();
    if(regReturn == false)
    {
        return false;
    }

    this->sq1.SetWatchdogTimer(0);
    this->sq2.SetWatchdogTimer(0);
    //TODO
    return true;
}

byte ApuMemoryMap::Read(dword address)
{
    return this->apuRegMem.Read(address);
}

void ApuMemoryMap::Write(dword address, byte value)
{
    this->apuRegMem.Write(address, value);

    switch(address)
    {
        case ApuRegisters::ApuAddresses::SQ1_VOL_ADDR:
        {
            this->sq1.SetDutyCycle(this->apuRegMem.name.SQ1.dutyNum);
            this->sq1.SetHaltWatchdogTimer(this->apuRegMem.name.SQ1.lengthCounterHault);
            this->sq1.SetConstantVolume(this->apuRegMem.name.SQ1.constantVolume);
            this->sq1.SetMaxVolumeOrEnvelopePeriod(this->apuRegMem.name.SQ1.volumeAndEnvelopePeriod);
            this->sq1.ResetVolumeDecayEnvelope();
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_SWEEP_ADDR:
        {
            this->sq1.SetFrequencySweep(this->apuRegMem.name.SQ1.enable, this->apuRegMem.name.SQ1.period, this->apuRegMem.name.SQ1.negative, this->apuRegMem.name.SQ1.shift);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_LO_ADDR:
        {
            dword upperVal = this->sq1.GetPulsePeriod() & 0xFF00;
            this->sq1.SetPulsePeriod(upperVal | this->apuRegMem.name.SQ1.LO);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ1_HI_ADDR:
        {
            dword lowerVal = this->sq1.GetPulsePeriod() & 0x00FF;
            dword period = (this->apuRegMem.name.SQ1.timerHigh << 8) | lowerVal;
            this->sq1.SetPulsePeriod(period);
            this->sq1.SetDutyCycle(0);
            this->sq1.ResetVolumeDecayEnvelope();
            this->sq1.SetWatchdogTimerFromCode(this->apuRegMem.name.SQ1.lengthCounter);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ2_VOL_ADDR:
        {
            this->sq2.SetDutyCycle(this->apuRegMem.name.SQ2.dutyNum);
            this->sq2.SetHaltWatchdogTimer(this->apuRegMem.name.SQ2.lengthCounterHault);
            this->sq2.SetConstantVolume(this->apuRegMem.name.SQ2.constantVolume);
            this->sq2.SetMaxVolumeOrEnvelopePeriod(this->apuRegMem.name.SQ2.volumeAndEnvelopePeriod);
            this->sq2.ResetVolumeDecayEnvelope();
            break;
        }
        case ApuRegisters::ApuAddresses::SQ2_SWEEP_ADDR:
        {
            this->sq2.SetFrequencySweep(this->apuRegMem.name.SQ2.enable, this->apuRegMem.name.SQ2.period, this->apuRegMem.name.SQ2.negative, this->apuRegMem.name.SQ2.shift);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ2_LO_ADDR:
        {
            dword upperVal = this->sq2.GetPulsePeriod() & 0xFF00;
            this->sq2.SetPulsePeriod(upperVal | this->apuRegMem.name.SQ2.LO);
            break;
        }
        case ApuRegisters::ApuAddresses::SQ2_HI_ADDR:
        {
            dword lowerVal = this->sq2.GetPulsePeriod() & 0x00FF;
            dword period = (this->apuRegMem.name.SQ2.timerHigh << 8) | lowerVal;
            this->sq2.SetPulsePeriod(period);
            this->sq2.SetDutyCycle(0);
            this->sq2.ResetVolumeDecayEnvelope();
            this->sq2.SetWatchdogTimerFromCode(this->apuRegMem.name.SQ2.lengthCounter);
            break;
        }
        case ApuRegisters::ApuAddresses::TRI_LINEAR_ADDR:
        {
            this->tri.SetLinearCounter(this->apuRegMem.name.TRI.linearCounter);
            this->tri.SetHaltTimers(this->apuRegMem.name.TRI.lengthCounterHalt);
            break;
        }
        case ApuRegisters::ApuAddresses::TRI_LO_ADDR:
        {
            dword upperVal = this->tri.GetPeriod() & 0xFF00;
            this->tri.SetPeriod(upperVal | this->apuRegMem.name.TRI.LO);
            break;
        }
        case ApuRegisters::ApuAddresses::TRI_HI_ADDR:
        {
            dword lowerVal = this->tri.GetPeriod() & 0x00FF;
            dword period = (this->apuRegMem.name.TRI.timerHigh << 8) | lowerVal;
            this->tri.SetPeriod(period);
            this->tri.SetWatchdogTimerFromCode(this->apuRegMem.name.TRI.lengthCounter);
            this->tri.TriggerLinearReset();
            break;
        }
        case ApuRegisters::ApuAddresses::NOISE_VOL_ADDR:
        {
            this->noise.SetMaxVolumeOrEnvelopePeriod(this->apuRegMem.name.NOISE.volumeAndEnvelopePeriod);
            this->noise.SetConstantVolume(this->apuRegMem.name.NOISE.constantVolume);
            this->noise.SetHaltWatchdogTimer(this->apuRegMem.name.NOISE.lengthCounterHault);
            this->noise.ResetVolumeDecayEnvelope();
            break;
        }
        case ApuRegisters::ApuAddresses::NOISE_PERIOD_ADDR:
        {
            this->noise.SetPeriodFromLookupTable(this->apuRegMem.name.NOISE.noisePeriod);
            this->noise.SetUseSixthBit(this->apuRegMem.name.NOISE.loopNoise);
            break;
        }
        case ApuRegisters::ApuAddresses::NOISE_LENGTH_COUNTER_ADDR:
        {
            this->noise.SetWatchdogTimerFromCode(this->apuRegMem.name.NOISE.lengthCounterLoad);
            this->noise.ResetVolumeDecayEnvelope();
            break;
        }
        case ApuRegisters::ApuAddresses::SND_CHN_ADDR:
        {
            bool sq1Enabled = this->apuRegMem.name.channels.pulse1;
            this->sq1.SetEnabled(sq1Enabled);
            if(sq1Enabled == false)
            {
                this->sq1.SetWatchdogTimer(0);
            }

            bool sq2Enabled = this->apuRegMem.name.channels.pulse2;
            this->sq2.SetEnabled(sq2Enabled);
            if(sq2Enabled == false)
            {
                this->sq2.SetWatchdogTimer(0);
            }
            bool triEnabled = this->apuRegMem.name.channels.triangle;
            this->tri.SetEnabled(triEnabled);
            if(triEnabled == false)
            {
                this->tri.SetWatchdogTimer(0);
            }
            bool noiseEnabled = this->apuRegMem.name.channels.noise;
            this->noise.SetEnabled(noiseEnabled);
            if(noiseEnabled)
            {
                this->noise.SetWatchdogTimer(0);
            }
            break;
        }
        case ApuRegisters::ApuAddresses::FRAME_COUNTER_ADDR:
        {
            //reset frame counter
            this->apuRegMem.vRegs.frameCounterSeqNum = 0;
            this->resetFrameCounter = true;
            break;
        }
    }
}

byte ApuMemoryMap::Seek(dword address) const
{
    return this->apuRegMem.Seek(address);
}

bool ApuMemoryMap::Contains(dword address) const
{
    if(address == JOYSTICK_STROBE_ADDRESS)
    {
        return false;
    }
    return IMemoryRW::Contains(address);
}

int ApuMemoryMap::GetCpuClockRateHz()
{
    return this->cpuClockRateHz;
}

int ApuMemoryMap::GetFrameCounterRateHz()
{
    return this->frameCounterRateHz;
}

const long long& ApuMemoryMap::GetTotalApuCycles() const
{
    return this->totalApuCycles;
}

void ApuMemoryMap::SetTotalApuCycles(const long long& value)
{
    this->totalApuCycles = value;
}

ApuRegisters& ApuMemoryMap::GetRegisters()
{
    return this->apuRegMem;
}

SquareWave& ApuMemoryMap::GetSquareWave1()
{
    return this->sq1;
}

SquareWave& ApuMemoryMap::GetSquareWave2()
{
    return this->sq2;
}

TriangleWave& ApuMemoryMap::GetTriangleWave()
{
    return this->tri;
}

NoiseWave& ApuMemoryMap::GetNoiseWave()
{
    return this->noise;
}

bool ApuMemoryMap::GetResetFrameCounter()
{
    return this->resetFrameCounter;
}
void ApuMemoryMap::SetResetFrameCounter( bool frameCounterReset )
{
    this->resetFrameCounter = frameCounterReset;
}