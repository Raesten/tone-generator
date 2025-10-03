#include "PadSynthesizer.h"

PadSynthesizer::PadSynthesizer()
{
    // Voices and sounds will be added in prepareToPlay()
    // This ensures proper initialization with the correct sample rate
}

void PadSynthesizer::prepare(double sampleRate, int samplesPerBlock)
{
    setCurrentPlaybackSampleRate(sampleRate);
    
    // Prepare all voices with the correct sample rate and block size
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->prepare(sampleRate, samplesPerBlock);
        }
    }
}

// Note: We now use JUCE's built-in Synthesiser::renderNextBlock method
// which automatically handles voice management and rendering

void PadSynthesizer::setDetuneAmount(float detuneAmount)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setDetuneAmount(detuneAmount);
        }
    }
}

void PadSynthesizer::setOscillatorCount(int count)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setOscillatorCount(count);
        }
    }
}

void PadSynthesizer::setEnvelopeAttack(float attack)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setEnvelopeAttack(attack);
        }
    }
}

void PadSynthesizer::setEnvelopeDecay(float decay)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setEnvelopeDecay(decay);
        }
    }
}

void PadSynthesizer::setEnvelopeSustain(float sustain)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setEnvelopeSustain(sustain);
        }
    }
}

void PadSynthesizer::setEnvelopeRelease(float release)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setEnvelopeRelease(release);
        }
    }
}

void PadSynthesizer::setFilterCutoff(float cutoff)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterCutoff(cutoff);
        }
    }
}

void PadSynthesizer::setFilterResonance(float resonance)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterResonance(resonance);
        }
    }
}

// Filter envelope controls
void PadSynthesizer::setFilterEnvelopeAmount(float amount)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterEnvelopeAmount(amount);
        }
    }
}

void PadSynthesizer::setFilterEnvelopeAttack(float attack)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterEnvelopeAttack(attack);
        }
    }
}

void PadSynthesizer::setFilterEnvelopeDecay(float decay)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterEnvelopeDecay(decay);
        }
    }
}

void PadSynthesizer::setFilterEnvelopeSustain(float sustain)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterEnvelopeSustain(sustain);
        }
    }
}

void PadSynthesizer::setFilterEnvelopeRelease(float release)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterEnvelopeRelease(release);
        }
    }
}

// LFO controls
void PadSynthesizer::setFilterLFODepth(float depth)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterLFODepth(depth);
        }
    }
}

void PadSynthesizer::setFilterLFORate(float rate)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setFilterLFORate(rate);
        }
    }
}

void PadSynthesizer::setPitchLFODepth(float depth)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setPitchLFODepth(depth);
        }
    }
}

void PadSynthesizer::setPitchLFORate(float rate)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setPitchLFORate(rate);
        }
    }
}

void PadSynthesizer::triggerNote(int midiNoteNumber, float velocity)
{
    noteOn(1, midiNoteNumber, velocity);
}

void PadSynthesizer::releaseNote(int midiNoteNumber)
{
    noteOff(1, midiNoteNumber, 1.0f, true);
}

void PadSynthesizer::releaseAllNotes()
{
    // Release all notes with tail-off (allowTailOff=true) for smooth release
    allNotesOff(1, true);
}

void PadSynthesizer::triggerChord(const std::vector<int>& midiNotes, float velocity)
{
    for (int note : midiNotes)
    {
        triggerNote(note, velocity);
    }
}

void PadSynthesizer::triggerMajorChord(int rootNote, float velocity)
{
    std::vector<int> chord = {rootNote, rootNote + 4, rootNote + 7};
    triggerChord(chord, velocity);
}

void PadSynthesizer::triggerMinorChord(int rootNote, float velocity)
{
    std::vector<int> chord = {rootNote, rootNote + 3, rootNote + 7};
    triggerChord(chord, velocity);
}

void PadSynthesizer::triggerSus2Chord(int rootNote, float velocity)
{
    std::vector<int> chord = {rootNote, rootNote + 2, rootNote + 7};
    triggerChord(chord, velocity);
}

void PadSynthesizer::triggerSus4Chord(int rootNote, float velocity)
{
    std::vector<int> chord = {rootNote, rootNote + 5, rootNote + 7};
    triggerChord(chord, velocity);
}

void PadSynthesizer::updateAllVoices()
{
    // This method can be used to update all voices with current parameters
    // Currently parameters are updated individually in the setter methods
}

void PadSynthesizer::setGlobalFrequency(double frequencyHz)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setGlobalFrequency(frequencyHz);
        }
    }
}

void PadSynthesizer::setGlobalWaveform(int waveformType)
{
    for (int i = 0; i < getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<PadVoice*>(getVoice(i)))
        {
            voice->setWaveform(waveformType);
        }
    }
}
