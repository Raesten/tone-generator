#include "PadSound.h"

PadSound::PadSound()
{
}

bool PadSound::appliesToNote(int midiNoteNumber)
{
    juce::ignoreUnused(midiNoteNumber);
    return true; // This sound can play any note
}

bool PadSound::appliesToChannel(int midiChannel)
{
    juce::ignoreUnused(midiChannel);
    return true; // This sound can play on any channel
}
