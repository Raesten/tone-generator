#include "ToneGenerator.h"
#include <cmath>

ToneGenerator::ToneGenerator()
{
}

ToneGenerator::~ToneGenerator()
{
}

void ToneGenerator::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    phase = 0.0;
}

void ToneGenerator::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!enabled)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getNumChannels() > 1 
                        ? bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample) 
                        : nullptr;
    
    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        float sampleValue = generateSample() * amplitude;
        
        leftChannel[sample] = sampleValue;
        if (rightChannel != nullptr)
            rightChannel[sample] = sampleValue;
            
        updatePhase();
    }
}

void ToneGenerator::reset()
{
    phase = 0.0;
}

void ToneGenerator::setFrequency(float newFrequency)
{
    frequency = juce::jlimit(20.0f, 20000.0f, newFrequency);
}

void ToneGenerator::setAmplitude(float newAmplitude)
{
    amplitude = juce::jlimit(0.0f, 1.0f, newAmplitude);
}

void ToneGenerator::setWaveType(WaveType newWaveType)
{
    waveType = newWaveType;
}

void ToneGenerator::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
    if (!enabled)
        phase = 0.0;
}

float ToneGenerator::generateSample()
{
    switch (waveType)
    {
        case WaveType::Sine:
            return static_cast<float>(std::sin(phase * 2.0 * juce::MathConstants<double>::pi));
            
        case WaveType::Square:
            return phase < 0.5 ? 1.0f : -1.0f;
            
        case WaveType::Sawtooth:
            return 2.0f * static_cast<float>(phase) - 1.0f;
            
        case WaveType::Triangle:
            if (phase < 0.5)
                return 4.0f * static_cast<float>(phase) - 1.0f;
            else
                return 3.0f - 4.0f * static_cast<float>(phase);
                
        default:
            return 0.0f;
    }
}

void ToneGenerator::updatePhase()
{
    phase += frequency / currentSampleRate;
    if (phase >= 1.0)
        phase -= 1.0;
}
