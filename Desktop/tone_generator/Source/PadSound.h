#pragma once

#include <JuceHeader.h>

class PadSound : public juce::SynthesiserSound
{
public:
    PadSound();
    ~PadSound() override = default;
    
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
};
