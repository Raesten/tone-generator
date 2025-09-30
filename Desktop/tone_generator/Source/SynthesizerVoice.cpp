#include "SynthesizerVoice.h"

SynthesizerVoice::SynthesizerVoice()
{
    // Initialize ADSR parameters
    adsrParams.attack = 0.1f;
    adsrParams.decay = 0.1f;
    adsrParams.sustain = 0.7f;
    adsrParams.release = 0.3f;
    adsr.setParameters(adsrParams);
    
    // Initialize filter
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setCutoffFrequency(1000.0f);
    filter.setResonance(0.1f);
}

bool SynthesizerVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthesizerVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    ignoreUnused(sound, currentPitchWheelPosition);
    
    frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    phase = 0.0;
    lfoPhase = 0.0;
    
    adsr.setParameters(adsrParams);
    adsr.noteOn();
}

void SynthesizerVoice::stopNote(float velocity, bool allowTailOff)
{
    ignoreUnused(velocity);
    
    if (allowTailOff)
        adsr.noteOff();
    else
        clearCurrentNote();
}

void SynthesizerVoice::pitchWheelMoved(int newPitchWheelValue)
{
    ignoreUnused(newPitchWheelValue);
    // Implement pitch wheel functionality if needed
}

void SynthesizerVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    ignoreUnused(controllerNumber, newControllerValue);
    // Implement MIDI controller functionality if needed
}

void SynthesizerVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isActive())
        return;
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float sampleValue = generateOscillatorSample();
        
        // Apply ADSR envelope
        float envelopeValue = adsr.getNextSample();
        sampleValue *= envelopeValue;
        
        // Apply filter
        sampleValue = filter.processSample(sampleValue, 0);
        
        // Apply LFO modulation
        updateLfo();
        updateFilter();
        
        // Write to all output channels
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, startSample + sample, sampleValue);
        }
        
        // Update phase
        phase += frequency / getSampleRate();
        if (phase >= 1.0)
            phase -= 1.0;
    }
    
    // Check if note should be released
    if (!adsr.isActive())
        clearCurrentNote();
}

void SynthesizerVoice::setWaveType(WaveType newWaveType)
{
    waveType = newWaveType;
}

void SynthesizerVoice::setAttackTime(float attackTimeMs)
{
    adsrParams.attack = attackTimeMs / 1000.0f;
    adsr.setParameters(adsrParams);
}

void SynthesizerVoice::setDecayTime(float decayTimeMs)
{
    adsrParams.decay = decayTimeMs / 1000.0f;
    adsr.setParameters(adsrParams);
}

void SynthesizerVoice::setSustainLevel(float sustainLevel)
{
    adsrParams.sustain = juce::jlimit(0.0f, 1.0f, sustainLevel);
    adsr.setParameters(adsrParams);
}

void SynthesizerVoice::setReleaseTime(float releaseTimeMs)
{
    adsrParams.release = releaseTimeMs / 1000.0f;
    adsr.setParameters(adsrParams);
}

void SynthesizerVoice::setFilterCutoff(float cutoffHz)
{
    filter.setCutoffFrequency(cutoffHz);
}

void SynthesizerVoice::setFilterResonance(float resonance)
{
    filter.setResonance(juce::jlimit(0.0f, 1.0f, resonance));
}

void SynthesizerVoice::setLfoRate(float rateHz)
{
    lfoRate = rateHz;
}

void SynthesizerVoice::setLfoDepth(float depth)
{
    lfoDepth = juce::jlimit(0.0f, 1.0f, depth);
}

float SynthesizerVoice::generateOscillatorSample()
{
    switch (waveType)
    {
        case WaveType::Sine:
            return std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
            
        case WaveType::Square:
            return phase < 0.5 ? 1.0f : -1.0f;
            
        case WaveType::Sawtooth:
            return 2.0f * static_cast<float>(phase) - 1.0f;
            
        case WaveType::Triangle:
            if (phase < 0.5)
                return 4.0f * static_cast<float>(phase) - 1.0f;
            else
                return 3.0f - 4.0f * static_cast<float>(phase);
                
        case WaveType::Noise:
            return generateNoiseSample();
            
        default:
            return 0.0f;
    }
}

float SynthesizerVoice::generateNoiseSample()
{
    return random.nextFloat() * 2.0f - 1.0f;
}

void SynthesizerVoice::updateLfo()
{
    lfoPhase += lfoRate / getSampleRate();
    if (lfoPhase >= 1.0)
        lfoPhase -= 1.0;
}

void SynthesizerVoice::updateFilter()
{
    float lfoValue = std::sin(lfoPhase * 2.0 * juce::MathConstants<double>::pi) * lfoDepth;
    float modulatedCutoff = filter.getCutoffFrequency() * (1.0f + lfoValue);
    filter.setCutoffFrequency(modulatedCutoff);
}
