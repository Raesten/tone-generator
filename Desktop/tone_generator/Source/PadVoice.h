#pragma once

#include <JuceHeader.h>
#include "PadSound.h"

class PadVoice : public juce::SynthesiserVoice
{
public:
    PadVoice();
    ~PadVoice() override = default;
    
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    // Pad-specific parameters
    void setDetuneAmount(float detuneAmount);
    void setOscillatorCount(int count);
    void setEnvelopeAttack(float attack);
    void setEnvelopeDecay(float decay);
    void setEnvelopeSustain(float sustain);
    void setEnvelopeRelease(float release);
    void setFilterCutoff(float cutoff);
    void setFilterResonance(float resonance);
    
    // Global controls
    void setGlobalFrequency(double frequencyHz);
    void setWaveform(int waveformType);
    
private:
    // Multiple oscillators for detuning
    static constexpr int maxOscillators = 8;
    juce::dsp::Oscillator<float> oscillators[maxOscillators];
    int oscillatorCount = 4;
    float detuneAmount = 0.1f; // Detune amount in semitones
    
    // Envelope
    juce::ADSR envelope;
    juce::ADSR::Parameters envelopeParams;
    
    // Filter
    juce::dsp::StateVariableTPTFilter<float> filter;
    
    // Voice state
    bool isActive = false;
    float currentVelocity = 0.0f;
    float currentFrequency = 0.0f;
    
    // Global controls
    double globalFrequency = 440.0;
    int currentWaveform = 0; // 0=Sine, 1=Square, 2=Triangle, 3=Saw
    
    // Helper methods
    void updateOscillatorFrequencies();
    void updateEnvelopeParameters();
    void updateFilterParameters();
};
