#include "PadVoice.h"

PadVoice::PadVoice()
{
    // Initialize unison oscillators with detuning and panning
    initializeUnisonOscillators();
    
    // Initialize envelope for Lush Pad A preset with smoother curves
    // Attack: 100ms, Decay: 100ms, Sustain: 0.8, Release: 300ms
    envelopeParams.attack = 0.1f;  // 100ms attack - more responsive
    envelopeParams.decay = 0.1f;   // 100ms decay - more responsive
    envelopeParams.sustain = 0.8f; // 0.8 sustain level - slightly higher
    envelopeParams.release = 0.3f; // 300ms release - much more responsive
    envelope.setParameters(envelopeParams);
    
    // Initialize envelope smoother for stable transitions
    envelopeSmoother.reset(0.0f);
    envelopeSmoother.setCurrentAndTargetValue(0.0f);
    lastEnvelopeLevel = 0.0f;
    
    // Initialize filter for Lush Pad A preset
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setCutoffFrequency(1200.0f); // Lush Pad A: 1.2 kHz cutoff
    filter.setResonance(0.6f); // Lush Pad A: Q 0.6
    
    // Initialize filter envelope for movement
    filterEnvelopeParams.attack = 0.1f;   // Quick attack
    filterEnvelopeParams.decay = 0.3f;    // Medium decay
    filterEnvelopeParams.sustain = 0.4f;  // Lower sustain for movement
    filterEnvelopeParams.release = 1.0f;  // Medium release
    filterEnvelope.setParameters(filterEnvelopeParams);
    
    // Initialize LFOs
    filterLFO.initialise([](float x) { return std::sin(x); }); // Sine wave LFO
    pitchLFO.initialise([](float x) { return std::sin(x); });  // Sine wave LFO
    
    // Initialize smoothed values for Lush Pad A preset
    smoothedFilterCutoff.reset(1200.0f); // Lush Pad A: 1.2 kHz cutoff
    smoothedFilterResonance.reset(0.6f); // Lush Pad A: Q 0.6
    smoothedDetuneAmount.reset(0.1f);
    smoothedFilterEnvelopeAmount.reset(100.0f); // Reduced envelope amount to prevent volume fluctuation
    smoothedFilterLFODepth.reset(50.0f); // Reduced LFO depth to prevent volume fluctuation
    smoothedPitchLFODepth.reset(1.0f); // Reduced pitch LFO depth
}

bool PadVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<PadSound*>(sound) != nullptr;
}

void PadVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    ignoreUnused(sound, currentPitchWheelPosition);
    
    DBG("=== PadVoice::startNote called ===");
    DBG("MIDI Note: " << midiNoteNumber << ", Velocity: " << velocity);
    DBG("Frequency: " << juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) << " Hz");
    
    currentVelocity = velocity;
    currentFrequency = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    
    updateOscillatorFrequencies();
    
    // Start envelopes
    envelope.noteOn();
    filterEnvelope.noteOn();
    isActive = true;
    
    DBG("Voice activated - isActive: " << (isActive ? "true" : "false"));
}

void PadVoice::stopNote(float velocity, bool allowTailOff)
{
    juce::ignoreUnused(velocity);
    
    if (allowTailOff)
    {
        envelope.noteOff();
        filterEnvelope.noteOff();
    }
    else
    {
        clearCurrentNote();
        envelope.reset();
        filterEnvelope.reset();
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
    
    // Performance optimization: Denormal protection for older devices
    juce::ScopedNoDenormals noDenormals;
    
    // Debug: Log render activity occasionally
    static int renderCounter = 0;
    if (++renderCounter % 1000 == 0) // Every 1000 render calls
    {
        DBG("=== PadVoice::renderNextBlock active ===");
        DBG("Frequency: " << currentFrequency << " Hz, Velocity: " << currentVelocity);
        DBG("Envelope active: " << (envelope.isActive() ? "true" : "false"));
    }
    
    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Generate unison oscillator output with detuning and panning
        float mixedSample = processUnisonSample();
        
        // Calculate dynamic filter cutoff with envelope and LFO using smoothed values
        float baseCutoff = smoothedFilterCutoff.getNextValue(); // Use smoothed cutoff
        
        // Add velocity mapping to cutoff for brightness expression
        float velocityCutoffModulation = currentVelocity * 1500.0f; // vel * 1500 Hz
        
        // Re-enable filter envelope and LFO with minimal depth for stability
        float envelopeModulation = 0.0f;
        float lfoModulation = 0.0f;
        
        // Calculate filter envelope modulation (minimal depth for stability)
        if (envelope.isActive()) {
            float envelopeLevel = envelope.getNextSample();
            envelopeModulation = envelopeLevel * smoothedFilterEnvelopeAmount.getNextValue() * 0.1f; // Minimal depth
        }
        
        // Calculate filter LFO modulation (minimal depth for stability)
        float lfoValue = filterLFO.processSample(0.0f);
        lfoModulation = lfoValue * smoothedFilterLFODepth.getNextValue() * 0.05f; // Very minimal depth
        
        // Debug logging occasionally
        static int debugCounter = 0;
        if (++debugCounter % 2000 == 0) {
            DBG("Filter modulation - EnvMod: " << envelopeModulation << ", LFOMod: " << lfoModulation << " (ENABLED - MINIMAL)");
        }
        
        // Calculate final cutoff frequency with all modulations (minimal depth for stability)
        float dynamicCutoff = juce::jlimit(100.0f, 8000.0f, 
            baseCutoff + velocityCutoffModulation + envelopeModulation + lfoModulation);
        filter.setCutoffFrequency(dynamicCutoff);
        
        // Update filter resonance with smoothed value
        filter.setResonance(smoothedFilterResonance.getNextValue());
        
        // Apply filter
        float filteredSample = filter.processSample(0, mixedSample);
        
        // Apply ADSR envelope with smoothing for stable transitions
        float rawEnvelopeLevel = envelope.getNextSample();
        
        // Smooth envelope transitions to prevent tremolo and artifacts
        envelopeSmoother.setTargetValue(rawEnvelopeLevel);
        float envelopeLevel = envelopeSmoother.getNextValue();
        
        // Apply smoothed envelope
        filteredSample *= envelopeLevel;
        
        // Apply velocity mapping to gain for loudness expression
        // Gain: gain *= 0.6 + 0.4 * velocity (0.6-1.0 range)
        float velocityGain = 0.6f + 0.4f * currentVelocity;
        filteredSample *= velocityGain * 0.3f; // Scale down for pad sound
        
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
    DBG("PadVoice::setDetuneAmount called with: " << newDetuneAmount);
    smoothedDetuneAmount.setTargetValue(newDetuneAmount);
    detuneAmount = newDetuneAmount; // Also update raw value for immediate use
    if (isActive)
        updateOscillatorFrequencies();
}

void PadVoice::setOscillatorCount(int count)
{
    DBG("PadVoice::setOscillatorCount called with: " << count);
    oscillatorCount = juce::jlimit(1, kOsc, count);
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
    smoothedFilterCutoff.setTargetValue(cutoff);
}

void PadVoice::setFilterResonance(float resonance)
{
    smoothedFilterResonance.setTargetValue(resonance);
}

void PadVoice::updateOscillatorFrequencies()
{
    updateUnisonDetuning();
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
    
    // Update all unison oscillators with the new waveform
    for (int i = 0; i < kOsc; ++i)
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

void PadVoice::prepare(double sampleRate, int samplesPerBlock)
{
    // Prepare the filter with the correct sample rate
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1; // Mono processing
    
    filter.prepare(spec);
    
    // Prepare LFOs
    filterLFO.prepare(spec);
    pitchLFO.prepare(spec);
    
    // Set LFO rates
    updateLFORates();
    
    // Configure smoothed values with 50ms smoothing time (buttery smooth)
    const float smoothingTimeMs = 50.0f; // 30-80ms range, using 50ms for buttery smooth
    smoothedFilterCutoff.reset(sampleRate, smoothingTimeMs / 1000.0f);
    smoothedFilterResonance.reset(sampleRate, smoothingTimeMs / 1000.0f);
    smoothedDetuneAmount.reset(sampleRate, smoothingTimeMs / 1000.0f);
    smoothedFilterEnvelopeAmount.reset(sampleRate, smoothingTimeMs / 1000.0f);
    smoothedFilterLFODepth.reset(sampleRate, smoothingTimeMs / 1000.0f);
    smoothedPitchLFODepth.reset(sampleRate, smoothingTimeMs / 1000.0f);
    
    // Configure envelope smoother for stable transitions (30ms smoothing)
    envelopeSmoother.reset(sampleRate, 0.03f);
    envelopeSmoother.setCurrentAndTargetValue(0.0f);
    
    // Reset all states
    filter.reset();
    filterEnvelope.reset();
    filterLFO.reset();
    pitchLFO.reset();
}

void PadVoice::initializeUnisonOscillators()
{
    // Initialize oscillators with sine wave (will be updated by setWaveform)
    for (int i = 0; i < kOsc; ++i)
    {
        oscillators[i].initialise([](float x) { return std::sin(x); });
    }
    
    // Set up panning positions for Lush Pad A: 40% spread
    pan[0] = -0.2f;  // -20%
    pan[1] = -0.1f;  // -10%
    pan[2] = +0.1f;  // +10%
    pan[3] = +0.2f;  // +20%
    pan[4] = -0.15f; // -15%
    pan[5] = +0.15f; // +15%
    
    // Set up ultra-minimal detuning for maximum stability: ±0.1c spread (extremely small)
    detune[0] = -0.1;  // -0.1 cents
    detune[1] = -0.05; // -0.05 cents
    detune[2] = +0.05; // +0.05 cents
    detune[3] = +0.1;  // +0.1 cents
    detune[4] = -0.075; // -0.075 cents
    detune[5] = +0.075; // +0.075 cents
    
    // Use phase-locked oscillators (all start with same phase)
    for (int i = 0; i < kOsc; ++i)
    {
        phase[i] = 0.0; // All oscillators start in phase
    }
    
    // Initialize precomputed pan gains for performance
    updatePanGains();
}

void PadVoice::updatePanGains()
{
    // Performance optimization: Precompute pan gains to avoid per-sample calculations
    for (int i = 0; i < kOsc; ++i)
    {
        // Simple left/right panning calculation
        leftPanGains[i] = (pan[i] <= 0.0f) ? (1.0f + pan[i]) : (1.0f - pan[i]);
        rightPanGains[i] = (pan[i] >= 0.0f) ? (1.0f - pan[i]) : (1.0f + pan[i]);
    }
    panGainsNeedUpdate = false;
}

void PadVoice::updateUnisonDetuning()
{
    // Always use the current note frequency for unison detuning
    // The global frequency is only used for the main frequency display
    float baseFreq = currentFrequency;
    
    // Add minimal pitch LFO modulation for subtle vibrato (very minimal depth)
    float pitchLFOValue = pitchLFO.processSample(0.0f);
    float pitchModulation = pitchLFOValue * smoothedPitchLFODepth.getNextValue() * 0.02f; // Very minimal depth
    baseFreq *= (1.0f + pitchModulation);
    
    DBG("PadVoice::updateUnisonDetuning - baseFreq: " << baseFreq << ", globalFreq: " << globalFrequency << ", currentFreq: " << currentFrequency);
    
    for (int i = 0; i < kOsc; ++i)
    {
        // Convert cents to frequency ratio
        double detuneRatio = std::pow(2.0, detune[i] / 1200.0);
        float detunedFreq = baseFreq * static_cast<float>(detuneRatio);
        
        DBG("Oscillator " << i << " detuned freq: " << detunedFreq << " (detune: " << detune[i] << " cents)");
        oscillators[i].setFrequency(detunedFreq);
    }
}

float PadVoice::processUnisonSample()
{
    // Performance optimization: Update pan gains only when needed
    if (panGainsNeedUpdate)
        updatePanGains();
    
    // ULTRA-STABLE UNISON SYSTEM - Minimal oscillators with ultra-minimal detuning
    float leftSample = 0.0f;
    float rightSample = 0.0f;
    
    // Use only 3 oscillators for maximum stability (was 6)
    int activeOscillators = juce::jlimit(1, 3, oscillatorCount);
    
    for (int i = 0; i < activeOscillators; ++i)
    {
        // Generate oscillator sample
        float oscValue = oscillators[i].processSample(0.0f);
        
        // Apply smooth crossfading based on active count
        float crossfadeGain = 1.0f;
        if (activeOscillators < kOsc)
        {
            // Smooth crossfade when reducing oscillator count
            crossfadeGain = 1.0f / std::sqrt(static_cast<float>(activeOscillators));
        }
        
        // Use precomputed pan gains for better performance
        leftSample += oscValue * leftPanGains[i] * crossfadeGain;
        rightSample += oscValue * rightPanGains[i] * crossfadeGain;
    }
    
    // Stable normalization for consistent volume
    float normalizationFactor = 1.0f / std::sqrt(static_cast<float>(activeOscillators));
    leftSample *= normalizationFactor;
    rightSample *= normalizationFactor;
    
    // Apply conservative gain for stability
    float unisonGain = 0.8f; // Slightly higher gain for better presence
    leftSample *= unisonGain;
    rightSample *= unisonGain;
    
    // Debug logging occasionally
    static int debugCounter = 0;
    if (++debugCounter % 1000 == 0) {
        DBG("processUnisonSample - oscillatorCount: " << oscillatorCount << ", leftSample: " << leftSample << ", rightSample: " << rightSample);
    }
    
    // Return mono mix (left + right) / 2
    return (leftSample + rightSample) * 0.5f;
}

// Filter envelope controls
void PadVoice::setFilterEnvelopeAmount(float amount)
{
    smoothedFilterEnvelopeAmount.setTargetValue(juce::jlimit(0.0f, 3000.0f, amount));
}

void PadVoice::setFilterEnvelopeAttack(float attack)
{
    filterEnvelopeParams.attack = attack;
    updateFilterEnvelopeParameters();
}

void PadVoice::setFilterEnvelopeDecay(float decay)
{
    filterEnvelopeParams.decay = decay;
    updateFilterEnvelopeParameters();
}

void PadVoice::setFilterEnvelopeSustain(float sustain)
{
    filterEnvelopeParams.sustain = sustain;
    updateFilterEnvelopeParameters();
}

void PadVoice::setFilterEnvelopeRelease(float release)
{
    filterEnvelopeParams.release = release;
    updateFilterEnvelopeParameters();
}

// LFO controls
void PadVoice::setFilterLFODepth(float depth)
{
    smoothedFilterLFODepth.setTargetValue(juce::jlimit(0.0f, 800.0f, depth));
}

void PadVoice::setFilterLFORate(float rate)
{
    filterLFORate = juce::jlimit(0.05f, 0.2f, rate);
    updateLFORates();
}

void PadVoice::setPitchLFODepth(float depth)
{
    smoothedPitchLFODepth.setTargetValue(juce::jlimit(0.0f, 12.0f, depth));
}

void PadVoice::setPitchLFORate(float rate)
{
    pitchLFORate = juce::jlimit(4.0f, 6.0f, rate);
    updateLFORates();
}

// Helper methods
void PadVoice::updateFilterEnvelopeParameters()
{
    filterEnvelope.setParameters(filterEnvelopeParams);
}

void PadVoice::updateLFORates()
{
    filterLFO.setFrequency(filterLFORate);
    pitchLFO.setFrequency(pitchLFORate);
}
