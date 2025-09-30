#pragma once

#include <JuceHeader.h>

class SynthesizerVoice : public juce::SynthesiserVoice
{
public:
    enum class WaveType
    {
        Sine,
        Square,
        Sawtooth,
        Triangle,
        Noise
    };
    
    SynthesizerVoice();
    ~SynthesizerVoice() override = default;
    
    // juce::SynthesiserVoice overrides
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    // Voice parameters
    void setWaveType(WaveType waveType);
    void setAttackTime(float attackTimeMs);
    void setDecayTime(float decayTimeMs);
    void setSustainLevel(float sustainLevel);
    void setReleaseTime(float releaseTimeMs);
    void setFilterCutoff(float cutoffHz);
    void setFilterResonance(float resonance);
    void setLfoRate(float rateHz);
    void setLfoDepth(float depth);
    
private:
    // Oscillator
    WaveType waveType = WaveType::Sine;
    double phase = 0.0;
    double frequency = 440.0;
    
    // ADSR Envelope
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    
    // Filter
    juce::dsp::StateVariableTPTFilter<float> filter;
    
    // LFO
    double lfoPhase = 0.0;
    float lfoRate = 1.0f;
    float lfoDepth = 0.0f;
    
    // Noise generator
    juce::Random random;
    
    // Helper methods
    float generateOscillatorSample();
    float generateNoiseSample();
    void updateLfo();
    void updateFilter();
};
