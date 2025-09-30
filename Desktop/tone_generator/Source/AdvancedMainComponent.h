#pragma once

#include <JuceHeader.h>
#include "SynthesizerVoice.h"

class AdvancedMainComponent : public juce::AudioAppComponent
{
public:
    AdvancedMainComponent();
    ~AdvancedMainComponent() override;

    // AudioAppComponent overrides
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // GUI overrides
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Synthesizer
    juce::Synthesiser synth;
    juce::SynthesiserSound synthSound;
    
    // UI Components - Oscillator Section
    juce::ComboBox waveTypeCombo;
    juce::Label waveTypeLabel;
    
    // UI Components - ADSR Section
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;
    
    // UI Components - Filter Section
    juce::Slider filterCutoffSlider, filterResonanceSlider;
    juce::Label filterCutoffLabel, filterResonanceLabel;
    
    // UI Components - LFO Section
    juce::Slider lfoRateSlider, lfoDepthSlider;
    juce::Label lfoRateLabel, lfoDepthLabel;
    
    // UI Components - Master Controls
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    juce::ToggleButton enableButton;
    
    // UI Components - Keyboard
    juce::MidiKeyboardComponent keyboard;
    
    // Helper methods
    void updateSynthesizerParameters();
    void setupUI();
    void connectSliders();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdvancedMainComponent)
};
