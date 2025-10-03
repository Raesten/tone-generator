/*
  ==============================================================================

    Synth.h
    Created: 19 Jul 2017 11:13:01am
    Author:  wrattchri1

  ==============================================================================
*/

#pragma once

#ifdef __APPLE__ 

#include "STK/BlitSquare.h"
#include "STK/BlitSaw.h"
#include "STK/Noise.h"
#include "STK/ADSR.h"
#include "DSPFilters/Dsp.h"

#elif defined _WIN32 || defined _WIN64

#include "STK\BlitSquare.h"
#include "STK\BlitSaw.h"
#include "STK\Noise.h"
#include "STK\ADSR.h"
#include "DSPFilters\Dsp.h"

#endif

#include "PluginProcessor.h"

class OscilSound : public juce::SynthesiserSound
{
public:
	OscilSound() {}
	bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
	bool appliesToChannel(int /*midiChannel*/) override { return true; }
	
};

class OscilVoice : public juce::SynthesiserVoice
{
public:
	OscilVoice() = default;
	~OscilVoice() override = default;
	bool canPlaySound(juce::SynthesiserSound* sound) override;
	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	bool isVoiceActive() const override { return isPlaying; }
        void pitchWheelMoved(int /*newValue*/) override {}
	void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}
	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
		processBlock(outputBuffer, startSample, numSamples);
	}

private:
	void processBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
	float applyLowPassFilter(float input);
	float applyStereoLowPassFilter(float input, bool isLeftChannel);
	float generateOscillator(float phase, float freq, float sampleRate, int oscType);

	// Multi-oscillator setup for lush, deep sound
	float currentFreq = 440.0f;
	float sampleRate = 44100.0f;
	bool isPlaying = false;
	int currentMidiNote = -1; // Track which MIDI note this voice is playing
	
	// Enhanced unison oscillators for lush, big sound (6 oscillators with stereo detuning)
	float osc1Phase = 0.0f, osc2Phase = 0.0f, osc3Phase = 0.0f, osc4Phase = 0.0f, osc5Phase = 0.0f, osc6Phase = 0.0f;
	float osc1Freq = 440.0f, osc2Freq = 440.0f, osc3Freq = 440.0f, osc4Freq = 440.0f, osc5Freq = 440.0f, osc6Freq = 440.0f;
	
	// Sub-oscillator for deep bass foundation (1 octave down)
	float subPhase = 0.0f;
	float subFreq = 220.0f; // One octave below base frequency
	
	// Envelope parameters (in seconds) - shared by all oscillators
	float ampAttack = 0.881f;  // 881ms attack
	float ampRelease = 0.924f; // 924ms release
	
	// Detune parameters for lush unison effect
	float detuneAmount = 0.06f; // 6 cents for warmth
	
	// Envelope state - shared by both oscillators
	float ampEnvelope = 0.0f;
	float noteOnTime = 0.0f;
	float noteOffTime = 0.0f;
	bool noteReleasing = false;
	
	// Low-pass filter for softening harsh frequencies
	float filterCutoff = 3000.0f; // Start with gentle cutoff
	float filterResonance = 0.3f; // Low resonance for smooth sound
	float filterState1 = 0.0f, filterState2 = 0.0f; // Filter state variables
	
	// Stereo filter state for left and right channels
	float leftFilterState1 = 0.0f, leftFilterState2 = 0.0f;
	float rightFilterState1 = 0.0f, rightFilterState2 = 0.0f;

	/*VOLUME_A_PARAM, VOLUME_D_PARAM, VOLUME_R_PARAM,
		FILTER_A_PARAM, FILTER_D_PARAM, FILTER_R_PARAM, VOLUME_S_PARAM, FILTER_S_PARAM,
		REVERB_PARAM, DELAY_PARAM, CHORUS_PARAM, COOL_EFFECT_PARAM,
		LFO_RATE_PARAM, LFO_AMP_PARAM, FILTER_CUTOFF_PARAM, FILTER_Q_PARAM, NUM_ALL_PARAMS*/
};
