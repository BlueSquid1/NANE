#include "VolumeEnvelope.h"


VolumeEnvelope::VolumeEnvelope()
{
    this->volumeEnvelopeSeq = std::make_unique<Sequencer>(-1, true, [this]{
        if(this->volumeDecayEnvelope <= 0)
        {
            if(this->isLooping == true)
            {
                this->volumeDecayEnvelope = 15;
            }
            return;
        }

        --this->volumeDecayEnvelope;
        return;
    });
}

void VolumeEnvelope::Clock()
{
    if(this->volumeResetFlag)
    {
        this->volumeDecayEnvelope = 15;
        this->volumeEnvelopeSeq->ResetCounterToPeriod();
        this->volumeResetFlag = false;
        return;
    }

    this->volumeEnvelopeSeq->Clock();
}

void VolumeEnvelope::ResetVolumeDecayEnvelope()
{
    this->volumeResetFlag = true;
}

void VolumeEnvelope::SetIsLooping(bool isLooping)
{
    this->isLooping = isLooping;
}

void VolumeEnvelope::SetMaxVolumeOrEnvelopePeriod(int volume)
{
    this->volumeEnvelopeSeq->SetPeriod(volume, false);
}

void VolumeEnvelope::SetConstantVolume(bool constantVol)
{
    this->constantVolume = constantVol;
}
float VolumeEnvelope::CalculateVolume()
{
    float multiplier = 0.0f;
    if(this->constantVolume)
    {
        // The period is used as the dirrect volume if constant volume is enabled:
        // https://wiki.nesdev.com/w/index.php/APU#Pulse_.28.244000-4007.29
        multiplier = this->volumeEnvelopeSeq->GetPeriod();
    }
    else
    {
        multiplier = this->volumeDecayEnvelope;
    }
    return multiplier;
}