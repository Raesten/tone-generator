#pragma once

#include <JuceHeader.h>

class ToneGenerator
{
public:
    enum class WaveType
    {
        Sine,
        Square,
        Sawtooth,
        Triangle
    };
    
    ToneGenerator();
    ~ToneGenerator();
    
    void prepare(double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void reset();
    
    void setFrequency(float frequency);
    void setAmplitude(float amplitude);
    void setWaveType(WaveType waveType);
    void setEnabled(bool enabled);
    
    float getFrequency() const { return frequency; }
    float getAmplitude() const { return amplitude; }
    WaveType getWaveType() const { return waveType; }
    bool isEnabled() const { return enabled; }

private:
    float frequency = 440.0f;
    float amplitude = 0.5f;
    WaveType waveType = WaveType::Sine;
    bool enabled = false;
    
    double currentSampleRate = 44100.0;
    double phase = 0.0;
    
    float generateSample();
    void updatePhase();
};
