#pragma once

#include <JuceHeader.h>
#include "Theme.h"
#include "LookAndFeelMinimal.h"

// Minimal test version of MainComponent to debug black screen issue
class MainComponent_Test : public juce::AudioAppComponent
{
public:
    MainComponent_Test()
    {
        // Apply minimal look and feel
        setLookAndFeel(&lookAndFeelMinimal);
        
        // Create a simple test button
        testButton.setButtonText("Test Button");
        testButton.setSize(200, 100);
        testButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
        testButton.setColour(juce::TextButton::textColourOffId, Theme::text);
        addAndMakeVisible(testButton);
        
        // Create a simple label
        testLabel.setText("Test Label", juce::dontSendNotification);
        testLabel.setFont(Theme::readout());
        testLabel.setColour(juce::Label::textColourId, Theme::text);
        testLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(testLabel);
        
        setSize(400, 300);
        // Don't initialize audio channels for now to avoid assertion
        // setAudioChannels(0, 2);
    }
    
    ~MainComponent_Test() override
    {
        setLookAndFeel(nullptr);
        // shutdownAudio();
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {}
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override 
    {
        bufferToFill.clearActiveBufferRegion();
    }
    void releaseResources() override {}

    void paint(juce::Graphics& g) override
    {
        // Simple background
        g.fillAll(Theme::bg);
        
        // Draw a simple rectangle to test drawing
        g.setColour(Theme::accent);
        g.fillRect(50, 50, 100, 100);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Center the button
        testButton.setBounds(bounds.withSizeKeepingCentre(200, 100).withY(50));
        
        // Center the label below the button
        testLabel.setBounds(bounds.withSizeKeepingCentre(300, 50).withY(170));
    }

private:
    LookAndFeelMinimal lookAndFeelMinimal;
    juce::TextButton testButton;
    juce::Label testLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent_Test)
};
