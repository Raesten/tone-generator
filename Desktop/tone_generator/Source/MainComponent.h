#pragma once

#include <JuceHeader.h>
#include "ToneGenerator.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ToneGenerator toneGenerator;
    
    juce::Slider frequencySlider;
    juce::Slider amplitudeSlider;
    juce::ToggleButton enableButton;
    juce::ComboBox waveTypeCombo;
    
    juce::Label frequencyLabel;
    juce::Label amplitudeLabel;
    juce::Label waveTypeLabel;
    
    void updateFrequency();
    void updateAmplitude();
    void updateWaveType();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
