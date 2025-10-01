#include "PadVoice.h"

PadVoice::PadVoice()
{
    // Initialize oscillators with different waveforms for richness
    for (int i = 0; i < maxOscillators; ++i)
    {
        // Mix of sine and sawtooth waves for pad character
        if (i % 2 == 0)
            oscillators[i].initialise([](float x) { return std::sin(x); });
        else
            oscillators[i].initialise([](float x) { return x / juce::MathConstants<float>::pi; });
    }
    
    // Initialize envelope with pad-like characteristics
    envelopeParams.attack = 0.5f;  // Slow attack for pad
    envelopeParams.decay = 0.3f;
    envelopeParams.sustain = 0.7f;
    envelopeParams.release = 2.0f; // Long release for pad
    envelope.setParameters(envelopeParams);
    
    // Initialize filter for warmth
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setCutoffFrequency(1000.0f);
    filter.setResonance(0.7f); // Higher resonance for more character
}

bool PadVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<PadSound*>(sound) != nullptr;
}

void PadVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    ignoreUnused(sound, currentPitchWheelPosition);
    
    currentVelocity = velocity;
    currentFrequency = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    
    updateOscillatorFrequencies();
    
    // Start envelope
    envelope.noteOn();
    isActive = true;
}

void PadVoice::stopNote(float velocity, bool allowTailOff)
{
    juce::ignoreUnused(velocity);
    
    if (allowTailOff)
    {
        envelope.noteOff();
    }
    else
    {
        clearCurrentNote();
        envelope.reset();
        isActive = false;
    }
}

void PadVoice::pitchWheelMoved(int newPitchWheelValue)
{
    juce::ignoreUnused(newPitchWheelValue);
    // Could implement pitch wheel functionality here
}

void PadVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    juce::ignoreUnused(controllerNumber, newControllerValue);
    // Could implement MIDI controller functionality here
}

void PadVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isActive)
        return;
    
    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Generate oscillator outputs with detuning
        float mixedSample = 0.0f;
        
        for (int i = 0; i < oscillatorCount; ++i)
        {
            float oscValue = oscillators[i].processSample(0.0f);
            mixedSample += oscValue / static_cast<float>(oscillatorCount);
        }
        
        // Apply filter BEFORE envelope for better sound shaping
        float filteredSample = filter.processSample(0, mixedSample);
        
        // Apply envelope
        float envelopeLevel = envelope.getNextSample();
        filteredSample *= envelopeLevel;
        
        // Apply velocity scaling
        filteredSample *= currentVelocity * 0.3f; // Scale down for pad sound
        
        // Add to output buffer for all channels
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, filteredSample);
        }
    }
    
    // Check if envelope is finished
    if (!envelope.isActive())
    {
        clearCurrentNote();
        isActive = false;
    }
}

void PadVoice::setDetuneAmount(float newDetuneAmount)
{
    detuneAmount = newDetuneAmount;
    if (isActive)
        updateOscillatorFrequencies();
}

void PadVoice::setOscillatorCount(int count)
{
    oscillatorCount = juce::jlimit(1, maxOscillators, count);
}

void PadVoice::setEnvelopeAttack(float attack)
{
    envelopeParams.attack = attack;
    updateEnvelopeParameters();
}

void PadVoice::setEnvelopeDecay(float decay)
{
    envelopeParams.decay = decay;
    updateEnvelopeParameters();
}

void PadVoice::setEnvelopeSustain(float sustain)
{
    envelopeParams.sustain = sustain;
    updateEnvelopeParameters();
}

void PadVoice::setEnvelopeRelease(float release)
{
    envelopeParams.release = release;
    updateEnvelopeParameters();
}

void PadVoice::setFilterCutoff(float cutoff)
{
    filter.setCutoffFrequency(cutoff);
}

void PadVoice::setFilterResonance(float resonance)
{
    filter.setResonance(resonance);
}

void PadVoice::updateOscillatorFrequencies()
{
    for (int i = 0; i < oscillatorCount; ++i)
    {
        // Calculate detuned frequency
        float detune = (i - oscillatorCount / 2.0f) * detuneAmount;
        // Use global frequency if available, otherwise use current note frequency
        float baseFreq = (globalFrequency > 0) ? static_cast<float>(globalFrequency) : currentFrequency;
        float detunedFreq = baseFreq * std::pow(2.0f, detune / 12.0f);
        
        oscillators[i].setFrequency(detunedFreq);
    }
}

void PadVoice::updateEnvelopeParameters()
{
    envelope.setParameters(envelopeParams);
}

void PadVoice::updateFilterParameters()
{
    // Filter parameters are set directly in setFilterCutoff/Resonance
}

void PadVoice::setGlobalFrequency(double frequencyHz)
{
    globalFrequency = frequencyHz;
    if (isActive)
    {
        updateOscillatorFrequencies();
    }
}

void PadVoice::setWaveform(int waveformType)
{
    currentWaveform = waveformType;
    
    // Update all oscillators with the new waveform
    for (int i = 0; i < maxOscillators; ++i)
    {
        switch (waveformType)
        {
            case 0: // Sine
                oscillators[i].initialise([](float x) { return std::sin(x); });
                break;
            case 1: // Square
                oscillators[i].initialise([](float x) { return x > 0.0f ? 1.0f : -1.0f; });
                break;
            case 2: // Triangle
                oscillators[i].initialise([](float x) { 
                    float normalized = (x + juce::MathConstants<float>::pi) / (2.0f * juce::MathConstants<float>::pi);
                    return 2.0f * std::abs(2.0f * (normalized - std::floor(normalized + 0.5f))) - 1.0f;
                });
                break;
            case 3: // Saw
                oscillators[i].initialise([](float x) { 
                    return (x + juce::MathConstants<float>::pi) / juce::MathConstants<float>::pi - 1.0f;
                });
                break;
        }
    }
}
