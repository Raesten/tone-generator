#pragma once

#include <JuceHeader.h>
#include "PadVoice.h"
#include "PadSound.h"

class PadSynthesizer : public juce::Synthesiser
{
public:
    PadSynthesizer();
    ~PadSynthesizer() override = default;
    
    void prepare(double sampleRate, int samplesPerBlock);
    
    // Pad-specific controls
    void setDetuneAmount(float detuneAmount);
    void setOscillatorCount(int count);
    void setEnvelopeAttack(float attack);
    void setEnvelopeDecay(float decay);
    void setEnvelopeSustain(float sustain);
    void setEnvelopeRelease(float release);
    void setFilterCutoff(float cutoff);
    void setFilterResonance(float resonance);
    
    // Filter envelope controls
    void setFilterEnvelopeAmount(float amount);
    void setFilterEnvelopeAttack(float attack);
    void setFilterEnvelopeDecay(float decay);
    void setFilterEnvelopeSustain(float sustain);
    void setFilterEnvelopeRelease(float release);
    
    // LFO controls
    void setFilterLFODepth(float depth);
    void setFilterLFORate(float rate);
    void setPitchLFODepth(float depth);
    void setPitchLFORate(float rate);
    
    // Note triggering
    void triggerNote(int midiNoteNumber, float velocity = 1.0f);
    void releaseNote(int midiNoteNumber);
    void releaseAllNotes();
    
    // Chord presets
    void triggerChord(const std::vector<int>& midiNotes, float velocity = 1.0f);
    void triggerMajorChord(int rootNote, float velocity = 1.0f);
    void triggerMinorChord(int rootNote, float velocity = 1.0f);
    void triggerSus2Chord(int rootNote, float velocity = 1.0f);
    void triggerSus4Chord(int rootNote, float velocity = 1.0f);
    
    // Global controls
    void setGlobalFrequency(double frequencyHz);
    void setGlobalWaveform(int waveformType);
    
private:
    int voiceCount = 8;
    
    void updateAllVoices();
};
