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
    
    // Global controls
    void setGlobalFrequency(double frequencyHz);
    void setWaveform(int waveformType);
    
    // DSP preparation
    void prepare(double sampleRate, int samplesPerBlock);
    
    // Performance monitoring
    bool isVoiceActive() const override { return isActive; }
    
private:
    // Unison oscillators with detuning and panning
    static constexpr int kOsc = 6; // 4-8 oscillators per voice
    std::array<juce::dsp::Oscillator<float>, kOsc> oscillators;
    std::array<float, kOsc> pan;        // -1..+1 panning
    std::array<double, kOsc> detune;    // in cents
    std::array<double, kOsc> phase;     // random initial phase
            int oscillatorCount = 3; // Ultra-stable preset: 3 oscillators
    float detuneAmount = 0.1f; // Detune amount in semitones (legacy)
    
    // Performance optimization: Precomputed pan gains to avoid per-sample mallocs
    std::array<float, kOsc> leftPanGains;   // Precomputed left channel gains
    std::array<float, kOsc> rightPanGains;  // Precomputed right channel gains
    bool panGainsNeedUpdate = true;         // Flag to update pan gains when needed
    
    // Envelope with smoothing
    juce::ADSR envelope;
    juce::ADSR::Parameters envelopeParams;
    juce::SmoothedValue<float> envelopeSmoother;  // Smooth envelope transitions
    float lastEnvelopeLevel = 0.0f;               // Previous envelope level for smoothing
    
    // Filter
    juce::dsp::StateVariableTPTFilter<float> filter;
    
    // Filter envelope for movement
    juce::ADSR filterEnvelope;
    juce::ADSR::Parameters filterEnvelopeParams;
    float filterEnvelopeAmount = 2000.0f; // +1-3 kHz envelope amount
    
    // LFO for evolving timbre
    juce::dsp::Oscillator<float> filterLFO;
    float filterLFODepth = 400.0f; // 200-800 Hz depth
    float filterLFORate = 0.12f;   // Lush Pad A: 0.12 Hz rate
    
    // Pitch LFO for vibrato
    juce::dsp::Oscillator<float> pitchLFO;
    float pitchLFODepth = 8.0f;    // ±5-12 cents depth
    float pitchLFORate = 5.0f;     // 4-6 Hz rate
    
    // Voice state
    bool isActive = false;
    float currentVelocity = 0.0f;
    float currentFrequency = 0.0f;
    
    // Global controls
    double globalFrequency = 440.0;
    int currentWaveform = 0; // 0=Sine, 1=Square, 2=Triangle, 3=Saw
    
    // Smoothed parameters to eliminate zipper noise
    juce::SmoothedValue<float> smoothedFilterCutoff;
    juce::SmoothedValue<float> smoothedFilterResonance;
    juce::SmoothedValue<float> smoothedDetuneAmount;
    juce::SmoothedValue<float> smoothedFilterEnvelopeAmount;
    juce::SmoothedValue<float> smoothedFilterLFODepth;
    juce::SmoothedValue<float> smoothedPitchLFODepth;
    
    // Helper methods
    void updateOscillatorFrequencies();
    void updateEnvelopeParameters();
    void updateFilterParameters();
    void updateFilterEnvelopeParameters();
    void updateLFORates();
    void initializeUnisonOscillators();
    void updateUnisonDetuning();
    void updatePanGains(); // Performance optimization: Precompute pan gains
    float processUnisonSample();
};
