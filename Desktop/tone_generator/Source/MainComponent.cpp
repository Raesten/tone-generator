#include "MainComponent.h"

MainComponent::MainComponent()
{
    // Set up frequency slider
    frequencySlider.setRange(20.0, 20000.0, 1.0);
    frequencySlider.setValue(440.0);
    frequencySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    frequencySlider.onValueChange = [this] { updateFrequency(); };
    addAndMakeVisible(frequencySlider);
    
    frequencyLabel.setText("Frequency (Hz)", juce::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    addAndMakeVisible(frequencyLabel);
    
    // Set up amplitude slider
    amplitudeSlider.setRange(0.0, 1.0, 0.01);
    amplitudeSlider.setValue(0.5);
    amplitudeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    amplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    amplitudeSlider.onValueChange = [this] { updateAmplitude(); };
    addAndMakeVisible(amplitudeSlider);
    
    amplitudeLabel.setText("Amplitude", juce::dontSendNotification);
    amplitudeLabel.attachToComponent(&amplitudeSlider, false);
    addAndMakeVisible(amplitudeLabel);
    
    // Set up wave type combo box
    waveTypeCombo.addItem("Sine", 1);
    waveTypeCombo.addItem("Square", 2);
    waveTypeCombo.addItem("Sawtooth", 3);
    waveTypeCombo.addItem("Triangle", 4);
    waveTypeCombo.setSelectedId(1);
    waveTypeCombo.onChange = [this] { updateWaveType(); };
    addAndMakeVisible(waveTypeCombo);
    
    waveTypeLabel.setText("Wave Type", juce::dontSendNotification);
    waveTypeLabel.attachToComponent(&waveTypeCombo, false);
    addAndMakeVisible(waveTypeLabel);
    
    // Set up enable button
    enableButton.setButtonText("Enable Tone");
    enableButton.setToggleState(false, juce::dontSendNotification);
    enableButton.onClick = [this] { toneGenerator.setEnabled(enableButton.getToggleState()); };
    addAndMakeVisible(enableButton);
    
    setSize(400, 300);
    setAudioChannels(0, 2);
    
    // Initialize tone generator
    updateFrequency();
    updateAmplitude();
    updateWaveType();
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int /* samplesPerBlockExpected */, double sampleRate)
{
    toneGenerator.prepare(sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    toneGenerator.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    toneGenerator.reset();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("Tone Generator", getLocalBounds().removeFromTop(50), 
                     juce::Justification::centred, 1);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Remove top area for title
    
    auto sliderArea = bounds.removeFromTop(120);
    auto leftSlider = sliderArea.removeFromLeft(getWidth() / 2);
    auto rightSlider = sliderArea;
    
    frequencySlider.setBounds(leftSlider.removeFromTop(100));
    amplitudeSlider.setBounds(rightSlider.removeFromTop(100));
    
    auto controlArea = bounds.removeFromTop(80);
    waveTypeCombo.setBounds(controlArea.removeFromLeft(150).removeFromTop(30));
    enableButton.setBounds(controlArea.removeFromLeft(150).removeFromTop(30));
}

void MainComponent::updateFrequency()
{
    toneGenerator.setFrequency(static_cast<float>(frequencySlider.getValue()));
}

void MainComponent::updateAmplitude()
{
    toneGenerator.setAmplitude(static_cast<float>(amplitudeSlider.getValue()));
}

void MainComponent::updateWaveType()
{
    int selectedId = waveTypeCombo.getSelectedId();
    ToneGenerator::WaveType waveType = ToneGenerator::WaveType::Sine;
    
    switch (selectedId)
    {
        case 1: waveType = ToneGenerator::WaveType::Sine; break;
        case 2: waveType = ToneGenerator::WaveType::Square; break;
        case 3: waveType = ToneGenerator::WaveType::Sawtooth; break;
        case 4: waveType = ToneGenerator::WaveType::Triangle; break;
    }
    
    toneGenerator.setWaveType(waveType);
}
