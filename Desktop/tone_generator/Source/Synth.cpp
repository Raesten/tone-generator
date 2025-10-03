/*
==============================================================================

Synth.cpp
Created: 19 Jul 2017 11:13:01am
Author:  wrattchri1

==============================================================================
*/

#include "Synth.h"
#include "Definitions.h"
#include "PluginProcessor.h"
#include <cmath>

//==============================================================================

// Constructor and destructor are now default - no custom implementation needed

bool OscilVoice::canPlaySound(juce::SynthesiserSound* sound)
{
	return dynamic_cast<OscilSound*> (sound) != nullptr;
}

void OscilVoice::startNote(int midiNoteNumber, float /*velocity*/,
        juce::SynthesiserSound* /*sound*/,
        int /*currentPitchWheelPosition*/)
{
        // Store the MIDI note number for this voice
        currentMidiNote = midiNoteNumber;
        
        // Configure multi-oscillator setup for lush, deep sound
        currentFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
        
        // Get detune amount from parameters
        auto& params = ParameterHolder::inst();
        float detuneAmount = params.parameters[VOLUME_A_PARAM].load(); // Use detune parameter
        
        // Enhanced detuning for bigger, lush sound
        float baseDetune = 0.08f + (detuneAmount * 0.15f); // Wider detuning range
        float detuneInCents = baseDetune * 100.0f; // Convert to cents
        
        // Create lush unison with stereo detuning
        osc1Freq = currentFreq; // Center frequency
        
        // Left side detuning (slightly flat)
        osc2Freq = currentFreq * std::pow(2.0f, -detuneInCents / 1200.0f);
        osc3Freq = currentFreq * std::pow(2.0f, -detuneInCents * 0.7f / 1200.0f);
        
        // Right side detuning (slightly sharp)
        osc4Freq = currentFreq * std::pow(2.0f, detuneInCents / 1200.0f);
        osc5Freq = currentFreq * std::pow(2.0f, detuneInCents * 0.7f / 1200.0f);
        
        // Center oscillator with subtle detune
        osc6Freq = currentFreq * std::pow(2.0f, detuneInCents * 0.3f / 1200.0f);
        
        // Sub-oscillator: One octave down for deep bass foundation
        subFreq = currentFreq * 0.5f; // One octave below
        
        // Reset all oscillator phases
        osc1Phase = 0.0f;
        osc2Phase = 0.0f;
        osc3Phase = 0.0f;
        osc4Phase = 0.0f;
        osc5Phase = 0.0f;
        osc6Phase = 0.0f;
        subPhase = 0.0f;
        
        // Initialize envelope state
        ampEnvelope = 0.01f; // Start with small value instead of 0 for immediate audio
        noteOnTime = 0.0f;
        noteReleasing = false;
        
        isPlaying = true;
        sampleRate = getSampleRate(); // Get the current sample rate from JUCE
        
        // Debug output for voice allocation
        DBG("=== VOICE STARTED ===");
        DBG("MIDI Note: " << midiNoteNumber << " (" << juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, true, 4) << ")");
        DBG("Voice ID: " << juce::String::toHexString((juce::pointer_sized_int)this) << " - Dual Osc Frequencies: " << osc1Freq << " Hz + " << osc2Freq << " Hz");
        // squareWave.setSampleRate(static_cast<float>(sampleRate));
        // sawWave.setSampleRate(static_cast<float>(sampleRate));
        // noise.setSampleRate(static_cast<float>(sampleRate));
        
        // Test 2: Enable ADSR setSampleRate calls - TESTING
        // adsr.setSampleRate(static_cast<float>(sampleRate));
        // filterEnvelope.setSampleRate(static_cast<float>(sampleRate));
        
        // Test 3: Enable ADSR setAllTimes calls with safe defaults - TESTING
        // adsr.setAllTimes(0.1f, 0.1f, 0.7f, 0.3f);
        // filterEnvelope.setAllTimes(0.1f, 0.1f, 0.7f, 0.3f);
        
        // Test 4: Enable keyOn calls - TESTING
        // adsr.keyOn();
        // filterEnvelope.keyOn();
        
        // Test 5: Enable oscillator setFrequency calls - TESTING
        // sineWave.setFrequency(currentFreq);
        // squareWave.setFrequency(currentFreq);
        // sawWave.setFrequency(currentFreq);
        
        // All STK calls in startNote are now working - the issue was in processBlock
        
        // Remaining STK calls disabled for debugging
        /*
        // Safe parameter access with fallbacks
        try {
            auto& params = ParameterHolder::inst();
            adsr.setAllTimes(params.parameters[VOLUME_A_PARAM].load(),
                            params.parameters[VOLUME_D_PARAM].load(),
                            params.parameters[VOLUME_S_PARAM].load(),
                            params.parameters[VOLUME_R_PARAM].load()
            );
            filterEnvelope.setAllTimes(params.parameters[FILTER_A_PARAM].load(),
                                    params.parameters[FILTER_D_PARAM].load(),
                                    params.parameters[FILTER_S_PARAM].load(),
                                    params.parameters[FILTER_R_PARAM].load()
            );
        } catch (...) {
            // Fallback to safe default values if parameter access fails
            adsr.setAllTimes(0.1f, 0.1f, 0.7f, 0.3f);
            filterEnvelope.setAllTimes(0.1f, 0.1f, 0.7f, 0.3f);
        }

	adsr.keyOn();
	filterEnvelope.keyOn();

	sineWave.setFrequency(currentFreq);
	squareWave.setFrequency(currentFreq);
	sawWave.setFrequency(currentFreq);
        */
}

void OscilVoice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
{
        // Start envelope release phase
        noteReleasing = true;
        noteOffTime = 0.0f; // Reset release timer
        // Note: don't set isPlaying = false immediately, let envelope finish release
        
        // Debug output for voice release
        DBG("=== VOICE STOPPING ===");
        DBG("MIDI Note: " << currentMidiNote << " (" << juce::MidiMessage::getMidiNoteName(currentMidiNote, true, true, 4) << ")");
        DBG("Voice ID: " << juce::String::toHexString((juce::pointer_sized_int)this) << " - Starting release phase");
}

void OscilVoice::processBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
        if (!isPlaying) {
            return;
        }
        
        // DUAL OSCILLATOR SYNTHESIS - Using warmth parameters
        for (int sample = 0; sample < numSamples; sample++)
        {
            // Get current parameters from our warmth sliders
            auto& params = ParameterHolder::inst();
            float currentAttack = params.parameters[VOLUME_A_PARAM].load();
            float currentRelease = params.parameters[VOLUME_R_PARAM].load();
            
            // Update envelope timers
            if (!noteReleasing) {
                noteOnTime += 1.0f / sampleRate;
            } else {
                noteOffTime += 1.0f / sampleRate;
            }
            
            // Calculate shared envelope using our warmth parameters
            if (!noteReleasing) {
                // Attack phase using our parameter
                if (noteOnTime < currentAttack) {
                    ampEnvelope = noteOnTime / currentAttack; // Linear attack
                } else {
                    ampEnvelope = 1.0f; // Sustain at full level
                }
            } else {
                // Release phase using our parameter
                float releaseProgress = noteOffTime / currentRelease;
                if (releaseProgress >= 1.0f) {
                    ampEnvelope = 0.0f;
                    isPlaying = false; // Stop voice when envelope is done
                    break;
                } else {
                    ampEnvelope = 1.0f - releaseProgress; // Linear release
                }
            }
            
            // Generate BIG, LUSH multi-oscillator sound with enhanced stereo width
            
            // Generate all 6 unison oscillators with different waveforms for richness
            float osc1Output = generateOscillator(osc1Phase, osc1Freq, sampleRate, 0); // Center: Triangle-Saw blend
            float osc2Output = generateOscillator(osc2Phase, osc2Freq, sampleRate, 1); // Left: Saw-heavy
            float osc3Output = generateOscillator(osc3Phase, osc3Freq, sampleRate, 2); // Left: Square wave
            float osc4Output = generateOscillator(osc4Phase, osc4Freq, sampleRate, 3); // Right: Triangle wave
            float osc5Output = generateOscillator(osc5Phase, osc5Freq, sampleRate, 4); // Right: Sine wave
            float osc6Output = generateOscillator(osc6Phase, osc6Freq, sampleRate, 5); // Center: Complex blend
            
            // Update all oscillator phases
            osc1Phase += osc1Freq / sampleRate; if (osc1Phase >= 1.0f) osc1Phase -= 1.0f;
            osc2Phase += osc2Freq / sampleRate; if (osc2Phase >= 1.0f) osc2Phase -= 1.0f;
            osc3Phase += osc3Freq / sampleRate; if (osc3Phase >= 1.0f) osc3Phase -= 1.0f;
            osc4Phase += osc4Freq / sampleRate; if (osc4Phase >= 1.0f) osc4Phase -= 1.0f;
            osc5Phase += osc5Freq / sampleRate; if (osc5Phase >= 1.0f) osc5Phase -= 1.0f;
            osc6Phase += osc6Freq / sampleRate; if (osc6Phase >= 1.0f) osc6Phase -= 1.0f;
            
            // Enhanced sub-oscillator for deep bass foundation
            float subOutput = std::sin(subPhase * 2.0f * juce::MathConstants<float>::pi);
            subOutput = std::tanh(subOutput * 1.2f) * 0.9f; // Stronger sub for big sound
            subPhase += subFreq / sampleRate;
            if (subPhase >= 1.0f) subPhase -= 1.0f;
            
            // Create lush stereo image with wide detuning
            // Left channel: oscillators 1, 2, 3 + sub
            float leftOutput = (osc1Output * 0.25f) + (osc2Output * 0.22f) + (osc3Output * 0.18f) + (subOutput * 0.12f);
            
            // Right channel: oscillators 1, 4, 5, 6 + sub
            float rightOutput = (osc1Output * 0.25f) + (osc4Output * 0.22f) + (osc5Output * 0.18f) + (osc6Output * 0.15f) + (subOutput * 0.12f);
            
            // Create stereo width and depth
            float mixedOutput = (leftOutput + rightOutput) * 0.5f;
            
            // Add subtle analog-style warmth to the mixed signal
            // Gentle compression and harmonic enhancement
            mixedOutput = std::tanh(mixedOutput * 0.8f) * 1.1f;
            
            // Apply stereo low-pass filters to soften harsh frequencies
            // Get filter cutoff from parameters (use FILTER_START_PARAM for cutoff control)
            float currentFilterCutoff = params.parameters[FILTER_START_PARAM].load();
            filterCutoff = juce::jlimit(200.0f, 8000.0f, currentFilterCutoff * 8000.0f); // Map 0-1 to 200-8000 Hz
            
            // Apply separate stereo filters for wider soundstage
            float filteredLeft = applyStereoLowPassFilter(leftOutput, true);
            float filteredRight = applyStereoLowPassFilter(rightOutput, false);
            
            // Apply shared envelope and moderate volume
            float leftFinal = filteredLeft * ampEnvelope * 0.15f; // Slightly increased for lushness
            float rightFinal = filteredRight * ampEnvelope * 0.15f;
            
            // Output to stereo channels with proper positioning
            if (buffer.getNumChannels() >= 2)
            {
                // Stereo output with spatial positioning
                buffer.addSample(0, startSample + sample, leftFinal);  // Left channel
                buffer.addSample(1, startSample + sample, rightFinal); // Right channel
            }
            else
            {
                // Mono fallback - mix both channels
                float monoOutput = (leftFinal + rightFinal) * 0.5f;
                buffer.addSample(0, startSample + sample, monoOutput);
            }
        }
        
        /* Original loop disabled for testing
        for (int sample = 0; sample < numSamples; sample++)
        {
            DBG("Inside sample loop: sample=" << sample << " of " << numSamples);
            
            // Generate simple sine wave
            float sineOutput = std::sin(testPhase * 2.0f * juce::MathConstants<float>::pi) * testVolume;
            
            // Debug first few samples
            static int sampleDebugCount = 0;
            if (sampleDebugCount < 5) {
                DBG("Sample " << sampleDebugCount << ": phase=" << testPhase << ", sine=" << sineOutput << ", channels=" << buffer.getNumChannels());
                sampleDebugCount++;
            }
            
            // Update phase
            testPhase += testFreq / sampleRate;
            if (testPhase >= 1.0f) testPhase -= 1.0f;
            
            // Output to all channels
            for (int channel = 0; channel < buffer.getNumChannels(); channel++)
            {
                buffer.addSample(channel, startSample + sample, sineOutput);
                
                // Debug first few channel writes
                static int channelDebugCount = 0;
                if (channelDebugCount < 3) {
                    DBG("Writing to channel " << channel << ", sample " << (startSample + sample) << ", value " << sineOutput);
                    channelDebugCount++;
                }
            }
        }
        */
        
        DBG("Finished processing " << numSamples << " samples");
        
        /* Original processing disabled for debugging
	const int numChannels = buffer.getNumChannels();
	float param;
	float filterStartFreq = ParameterHolder::inst().parameters[FILTER_START_PARAM].load();
	float filterEndFreq = ParameterHolder::inst().parameters[FILTER_END_PARAM].load();
	int currentOsc = ParameterHolder::inst().currentOsc;

	for (int sample = 0; sample < numSamples; sample++)
	{
                float curOutput = 0.0f;
		//need to get this into polymorphic structure?
		switch (currentOsc)
		{
		case OSC_SINE:
			curOutput = sineWave.tick();
			break;
		case OSC_SQUARE:
			curOutput = squareWave.tick();
			break;
		case OSC_SAW:
			curOutput = sawWave.tick();
			break;
		case OSC_NOISE:
			curOutput = noise.tick();
			break;
		}
		curOutput *= adsr.tick();
		//filter. need to run tick every sample but not change filter freq. Could implement lerp... No needs to be special lerp filter for that
		param = filterEnvelope.tick();

		//change paramaters here- not dependant on block size, is dependant on SR...
		//moving to modulo for checks took 7% off my CPU time!!
		if (f != nullptr && paramIncrement == 0)
		{
			param = mapFilterParam(param, filterStartFreq, filterEndFreq) + currentFreq;
			//check needed to avoid filter shutting down plugin with argument above nyquist
			if (param > 20000)		param = 20000;
			else if(param <= 20)	param = 20;
			f->setParam(2, param);
			paramIncrement = PARAM_UPDATE_SPEED;
		}
		else
		{
			paramIncrement--;
		}

		//some gross stuff to deal with DSP lib formatting.
		if (f != nullptr) {
			float* curSampArray[8]; // Use larger fixed size to accommodate various channel counts
			for (int channel = numChannels; --channel >= 0;) 
			{
				if (channel < 8) // Bounds check
					curSampArray[channel] = &curOutput;
			}
			f->process(1, curSampArray);
		}

		for (int channel = 0; channel < numChannels; channel++)
		{
			buffer.addSample(channel, startSample, curOutput);
		}
		++startSample;
	}
}

//returns the frequency mapped between the start and end freqs
float OscilVoice::mapFilterParam(float p_adsrAmp, float p_startFreq, float p_endFreq)
{
	float freqRange = 19980.0;
	//find min and max and scale them into 0-1
	float minVal = p_startFreq <= p_endFreq ? p_startFreq / freqRange : p_endFreq / freqRange;
	float maxVal = p_startFreq >= p_endFreq ? p_startFreq / freqRange : p_endFreq / freqRange;
	float difference = maxVal - minVal;

	//Set output based on whether the start or end is higher pitched
	if (p_startFreq < p_endFreq)
	{
		return (p_adsrAmp * difference + minVal) * freqRange;
	}
	else if (p_startFreq > p_endFreq)
	{
		return (maxVal - (p_adsrAmp * difference)) * freqRange;
	}
	else
		return p_startFreq; //no difference
        */
}

// Generate different oscillator waveforms for lush sound
float OscilVoice::generateOscillator(float phase, float freq, float sampleRate, int oscType)
{
    float output = 0.0f;
    
    switch (oscType) {
        case 0: // Center: Triangle-Saw blend (warm, fundamental)
            {
                float tri = (phase < 0.5f) ? (phase * 4.0f) - 1.0f : 3.0f - (phase * 4.0f);
                float saw = (phase * 2.0f) - 1.0f;
                output = (tri * 0.7f) + (saw * 0.3f);
                output = std::tanh(output * 1.1f) * 0.95f;
            }
            break;
            
        case 1: // Left: Saw-heavy (harmonic richness)
            {
                float tri = (phase < 0.5f) ? (phase * 4.0f) - 1.0f : 3.0f - (phase * 4.0f);
                float saw = (phase * 2.0f) - 1.0f;
                output = (tri * 0.4f) + (saw * 0.6f);
                output = std::tanh(output * 1.3f) * 0.9f;
            }
            break;
            
        case 2: // Left: Square wave (additional harmonics)
            output = (phase < 0.5f) ? 1.0f : -1.0f;
            output = std::tanh(output * 0.9f) * 0.8f;
            break;
            
        case 3: // Right: Triangle wave (smooth, warm)
            output = (phase < 0.5f) ? (phase * 4.0f) - 1.0f : 3.0f - (phase * 4.0f);
            output = std::tanh(output * 1.0f) * 0.85f;
            break;
            
        case 4: // Right: Sine wave (pure, smooth)
            output = std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
            output = std::tanh(output * 1.1f) * 0.75f;
            break;
            
        case 5: // Center: Complex blend (rich harmonics)
            {
                float tri = (phase < 0.5f) ? (phase * 4.0f) - 1.0f : 3.0f - (phase * 4.0f);
                float saw = (phase * 2.0f) - 1.0f;
                float square = (phase < 0.5f) ? 1.0f : -1.0f;
                output = (tri * 0.5f) + (saw * 0.3f) + (square * 0.2f);
                output = std::tanh(output * 1.2f) * 0.88f;
            }
            break;
    }
    
    return output;
}

// Simple 2-pole low-pass filter implementation
float OscilVoice::applyLowPassFilter(float input)
{
    // Calculate filter coefficients based on cutoff frequency and sample rate
    float omega = 2.0f * juce::MathConstants<float>::pi * filterCutoff / sampleRate;
    float cosOmega = std::cos(omega);
    float sinOmega = std::sin(omega);
    float alpha = sinOmega / (2.0f * filterResonance);
    
    // Calculate filter coefficients
    float b0 = (1.0f - cosOmega) / 2.0f;
    float b1 = 1.0f - cosOmega;
    float b2 = (1.0f - cosOmega) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosOmega;
    float a2 = 1.0f - alpha;
    
    // Normalize coefficients
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    
    // Apply filter
    float output = b0 * input + b1 * filterState1 + b2 * filterState2 - a1 * filterState1 - a2 * filterState2;
    
    // Update filter state
    filterState2 = filterState1;
    filterState1 = output;
    
    return output;
}

// Stereo low-pass filter with separate state for left/right channels
float OscilVoice::applyStereoLowPassFilter(float input, bool isLeftChannel)
{
    // Calculate filter coefficients based on cutoff frequency and sample rate
    float omega = 2.0f * juce::MathConstants<float>::pi * filterCutoff / sampleRate;
    float cosOmega = std::cos(omega);
    float sinOmega = std::sin(omega);
    float alpha = sinOmega / (2.0f * filterResonance);
    
    // Calculate filter coefficients
    float b0 = (1.0f - cosOmega) / 2.0f;
    float b1 = 1.0f - cosOmega;
    float b2 = (1.0f - cosOmega) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosOmega;
    float a2 = 1.0f - alpha;
    
    // Normalize coefficients
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    
    // Apply filter with separate state for left/right channels
    float output;
    if (isLeftChannel) {
        output = b0 * input + b1 * leftFilterState1 + b2 * leftFilterState2 - a1 * leftFilterState1 - a2 * leftFilterState2;
        // Update left filter state
        leftFilterState2 = leftFilterState1;
        leftFilterState1 = output;
    } else {
        output = b0 * input + b1 * rightFilterState1 + b2 * rightFilterState2 - a1 * rightFilterState1 - a2 * rightFilterState2;
        // Update right filter state
        rightFilterState2 = rightFilterState1;
        rightFilterState1 = output;
    }
    
    return output;
}
