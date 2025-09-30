#include "AdvancedMainComponent.h"

AdvancedMainComponent::AdvancedMainComponent()
    : synthSound(), keyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setupUI();
    connectSliders();
    
    // Add voices to synthesizer
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthesizerVoice());
    
    synth.addSound(&synthSound);
    
    // Set up audio
    setSize(800, 600);
    setAudioChannels(0, 2);
    
    updateSynthesizerParameters();
}

AdvancedMainComponent::~AdvancedMainComponent()
{
    shutdownAudio();
}

void AdvancedMainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void AdvancedMainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    if (enableButton.getToggleState())
    {
        synth.renderNextBlock(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
        
        // Apply master volume
        float masterVolume = static_cast<float>(masterVolumeSlider.getValue());
        bufferToFill.buffer->applyGain(masterVolume);
    }
}

void AdvancedMainComponent::releaseResources()
{
    synth.allNotesOff(0, true);
}

void AdvancedMainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Advanced Synthesizer", getLocalBounds().removeFromTop(50), 
                     juce::Justification::centred, 1);
}

void AdvancedMainComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Title area
    
    // Keyboard at bottom
    keyboard.setBounds(bounds.removeFromBottom(120));
    
    // Main controls area
    auto controlArea = bounds;
    auto leftColumn = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto rightColumn = controlArea;
    
    // Left column - Oscillator and ADSR
    auto oscArea = leftColumn.removeFromTop(120);
    waveTypeCombo.setBounds(oscArea.removeFromTop(30));
    
    auto adsrArea = leftColumn.removeFromTop(200);
    auto adsrRow1 = adsrArea.removeFromTop(40);
    auto adsrRow2 = adsrArea.removeFromTop(40);
    
    attackSlider.setBounds(adsrRow1.removeFromLeft(adsrRow1.getWidth() / 2));
    decaySlider.setBounds(adsrRow1);
    sustainSlider.setBounds(adsrRow2.removeFromLeft(adsrRow2.getWidth() / 2));
    releaseSlider.setBounds(adsrRow2);
    
    // Right column - Filter, LFO, and Master
    auto filterArea = rightColumn.removeFromTop(120);
    auto filterRow1 = filterArea.removeFromTop(40);
    auto filterRow2 = filterArea.removeFromTop(40);
    
    filterCutoffSlider.setBounds(filterRow1.removeFromLeft(filterRow1.getWidth() / 2));
    filterResonanceSlider.setBounds(filterRow1);
    
    auto lfoArea = rightColumn.removeFromTop(120);
    auto lfoRow1 = lfoArea.removeFromTop(40);
    auto lfoRow2 = lfoArea.removeFromTop(40);
    
    lfoRateSlider.setBounds(lfoRow1.removeFromLeft(lfoRow1.getWidth() / 2));
    lfoDepthSlider.setBounds(lfoRow1);
    
    auto masterArea = rightColumn.removeFromTop(80);
    masterVolumeSlider.setBounds(masterArea.removeFromTop(40));
    enableButton.setBounds(masterArea.removeFromTop(30));
}

void AdvancedMainComponent::setupUI()
{
    // Oscillator controls
    waveTypeCombo.addItem("Sine", 1);
    waveTypeCombo.addItem("Square", 2);
    waveTypeCombo.addItem("Sawtooth", 3);
    waveTypeCombo.addItem("Triangle", 4);
    waveTypeCombo.addItem("Noise", 5);
    waveTypeCombo.setSelectedId(1);
    addAndMakeVisible(waveTypeCombo);
    
    waveTypeLabel.setText("Wave Type", juce::dontSendNotification);
    waveTypeLabel.attachToComponent(&waveTypeCombo, false);
    addAndMakeVisible(waveTypeLabel);
    
    // ADSR controls
    attackSlider.setRange(0.0, 2000.0, 1.0);
    attackSlider.setValue(100.0);
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(attackSlider);
    
    attackLabel.setText("Attack (ms)", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, false);
    addAndMakeVisible(attackLabel);
    
    decaySlider.setRange(0.0, 2000.0, 1.0);
    decaySlider.setValue(100.0);
    decaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(decaySlider);
    
    decayLabel.setText("Decay (ms)", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, false);
    addAndMakeVisible(decayLabel);
    
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(sustainSlider);
    
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.attachToComponent(&sustainSlider, false);
    addAndMakeVisible(sustainLabel);
    
    releaseSlider.setRange(0.0, 5000.0, 1.0);
    releaseSlider.setValue(300.0);
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(releaseSlider);
    
    releaseLabel.setText("Release (ms)", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, false);
    addAndMakeVisible(releaseLabel);
    
    // Filter controls
    filterCutoffSlider.setRange(20.0, 20000.0, 1.0);
    filterCutoffSlider.setValue(1000.0);
    filterCutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterCutoffSlider);
    
    filterCutoffLabel.setText("Cutoff (Hz)", juce::dontSendNotification);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, false);
    addAndMakeVisible(filterCutoffLabel);
    
    filterResonanceSlider.setRange(0.0, 1.0, 0.01);
    filterResonanceSlider.setValue(0.1);
    filterResonanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(filterResonanceSlider);
    
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    filterResonanceLabel.attachToComponent(&filterResonanceSlider, false);
    addAndMakeVisible(filterResonanceLabel);
    
    // LFO controls
    lfoRateSlider.setRange(0.1, 20.0, 0.1);
    lfoRateSlider.setValue(1.0);
    lfoRateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfoRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(lfoRateSlider);
    
    lfoRateLabel.setText("LFO Rate (Hz)", juce::dontSendNotification);
    lfoRateLabel.attachToComponent(&lfoRateSlider, false);
    addAndMakeVisible(lfoRateLabel);
    
    lfoDepthSlider.setRange(0.0, 1.0, 0.01);
    lfoDepthSlider.setValue(0.0);
    lfoDepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfoDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(lfoDepthSlider);
    
    lfoDepthLabel.setText("LFO Depth", juce::dontSendNotification);
    lfoDepthLabel.attachToComponent(&lfoDepthSlider, false);
    addAndMakeVisible(lfoDepthLabel);
    
    // Master controls
    masterVolumeSlider.setRange(0.0, 1.0, 0.01);
    masterVolumeSlider.setValue(0.5);
    masterVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(masterVolumeSlider);
    
    masterVolumeLabel.setText("Master Volume", juce::dontSendNotification);
    masterVolumeLabel.attachToComponent(&masterVolumeSlider, false);
    addAndMakeVisible(masterVolumeLabel);
    
    enableButton.setButtonText("Enable Synthesizer");
    enableButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(enableButton);
    
    // Keyboard
    addAndMakeVisible(keyboard);
}

void AdvancedMainComponent::connectSliders()
{
    // Connect all sliders to update synthesizer parameters
    auto updateParams = [this] { updateSynthesizerParameters(); };
    
    attackSlider.onValueChange = updateParams;
    decaySlider.onValueChange = updateParams;
    sustainSlider.onValueChange = updateParams;
    releaseSlider.onValueChange = updateParams;
    filterCutoffSlider.onValueChange = updateParams;
    filterResonanceSlider.onValueChange = updateParams;
    lfoRateSlider.onValueChange = updateParams;
    lfoDepthSlider.onValueChange = updateParams;
    masterVolumeSlider.onValueChange = updateParams;
    
    waveTypeCombo.onChange = updateParams;
    enableButton.onClick = updateParams;
}

void AdvancedMainComponent::updateSynthesizerParameters()
{
    // Update all voices with current parameter values
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthesizerVoice*>(synth.getVoice(i)))
        {
            // Wave type
            int selectedId = waveTypeCombo.getSelectedId();
            SynthesizerVoice::WaveType waveType = SynthesizerVoice::WaveType::Sine;
            switch (selectedId)
            {
                case 1: waveType = SynthesizerVoice::WaveType::Sine; break;
                case 2: waveType = SynthesizerVoice::WaveType::Square; break;
                case 3: waveType = SynthesizerVoice::WaveType::Sawtooth; break;
                case 4: waveType = SynthesizerVoice::WaveType::Triangle; break;
                case 5: waveType = SynthesizerVoice::WaveType::Noise; break;
            }
            voice->setWaveType(waveType);
            
            // ADSR
            voice->setAttackTime(static_cast<float>(attackSlider.getValue()));
            voice->setDecayTime(static_cast<float>(decaySlider.getValue()));
            voice->setSustainLevel(static_cast<float>(sustainSlider.getValue()));
            voice->setReleaseTime(static_cast<float>(releaseSlider.getValue()));
            
            // Filter
            voice->setFilterCutoff(static_cast<float>(filterCutoffSlider.getValue()));
            voice->setFilterResonance(static_cast<float>(filterResonanceSlider.getValue()));
            
            // LFO
            voice->setLfoRate(static_cast<float>(lfoRateSlider.getValue()));
            voice->setLfoDepth(static_cast<float>(lfoDepthSlider.getValue()));
        }
    }
}
