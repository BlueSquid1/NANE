#pragma once

#include <memory>

#include "Sequencer.h"

class VolumeEnvelope
{
    private:
    std::unique_ptr<Sequencer> volumeEnvelopeSeq;
    bool volumeResetFlag = false; // tracks when a volume adjust has just happened
    bool constantVolume = false; //true: channel has constant volume. false: volume has sawtooth envelope.
    int volumeDecayEnvelope = 15; //used to track sawtooth envelope.
    bool isLooping;

    public:
    VolumeEnvelope();
    void Clock();
    void ResetVolumeDecayEnvelope();

    void SetIsLooping(bool isLooping);
    void SetMaxVolumeOrEnvelopePeriod(int volume);
    void SetConstantVolume(bool constantVol);
    float CalculateVolume();
};