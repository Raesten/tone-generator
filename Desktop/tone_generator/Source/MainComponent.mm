#include "MainComponent.h"
#include <algorithm>

#if JUCE_IOS
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#endif

MainComponent::MainComponent()
{
    // Apply minimal look and feel
    setLookAndFeel(&lookAndFeelMinimal);
    
    setupFlexBoxLayout();
    setupFrequencyControls();
    setupWaveformControls();
    setupVolumeControls();
    setupNoteButtons();
    setupChordButtons();
    setupAdvancedControls();
    setupPlayStopButton();
    setupUIFeedback();
    
    // Set initial window size - responsive design with more space
    setSize(1000, 800); // Larger default to accommodate better spacing
    
    // Enable keyboard input for debugging
    setWantsKeyboardFocus(true);
    
    // Initialize audio safely with JUCE defaults and fallback
    std::cout << "=== MainComponent constructor: About to call initialiseAudioSafely() ===" << std::endl;
    std::cout.flush();
    initialiseAudioSafely();
    std::cout << "=== MainComponent constructor: initialiseAudioSafely() completed ===" << std::endl;
    std::cout.flush();
    
    // Initialize synthesizer parameters
    updateAdvancedControls();
    
    // Initialize state persistence
    initializePropertiesFile();
    loadState();
    
    // Set up accessibility focus order
    setupAccessibilityFocusOrder();
}

MainComponent::~MainComponent()
{
    // Save state before shutting down
    saveState();
    
    setLookAndFeel(nullptr);
    shutdownAudio();
    
#if JUCE_IOS
    // Clean up audio notification observers
    [[NSNotificationCenter defaultCenter] removeObserver:(__bridge id)this name:AVAudioSessionRouteChangeNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:(__bridge id)this name:AVAudioSessionInterruptionNotification object:nil];
#endif
}

void MainComponent::setupFlexBoxLayout()
{
    // Main layout - vertical stack
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    mainFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    mainFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
}

void MainComponent::setupNoteButtons()
{
    // Set up note grid layout
    noteGridFlexBox.flexDirection = juce::FlexBox::Direction::row;
    noteGridFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    noteGridFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    noteGridFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    
    // Create and configure 12 note toggle buttons (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
    // Each button can be toggled on/off to form custom chords
    // Initial size will be set dynamically in updateButtonSizes()
    for (int i = 0; i < 12; ++i)
    {
        noteButtons[i].setButtonText(noteNames[i]);
        noteButtons[i].setClickingTogglesState(true); // Enable toggle behavior - buttons stay "down" when active
        noteButtons[i].onClick = [this, i] { onNoteButtonClicked(i); };
        
        // Theme-based styling - colors are set by LookAndFeel
        // Additional custom styling for special states using theme colors
        noteButtons[i].setColour(juce::TextButton::buttonColourId, Theme::noteOff);
        noteButtons[i].setColour(juce::TextButton::buttonOnColourId, Theme::noteSelected);
        noteButtons[i].setColour(juce::TextButton::textColourOffId, Theme::text);
        noteButtons[i].setColour(juce::TextButton::textColourOnId, juce::Colour(0xff000000));
        
        // VoiceOver accessibility
        noteButtons[i].setAccessible(true);
        
        playPanel.addAndMakeVisible(noteButtons[i]);
    }
    
    // Set initial button sizes
    updateButtonSizes();
}

void MainComponent::setupChordButtons()
{
    // Set up chord buttons layout
    chordFlexBox.flexDirection = juce::FlexBox::Direction::row;
    chordFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    chordFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    chordFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    
    // Configure chord buttons with theme styling
    cMajorButton.setButtonText("C Major");
    cMajorButton.setSize(120, 50);
    cMajorButton.onClick = [this] { padSynthesizer.triggerMajorChord(60, 0.8f); };
    cMajorButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
    cMajorButton.setColour(juce::TextButton::textColourOffId, Theme::text);
    cMajorButton.setAccessible(true);
    playPanel.addAndMakeVisible(cMajorButton);
    
    cMinorButton.setButtonText("C Minor");
    cMinorButton.setSize(120, 50);
    cMinorButton.onClick = [this] { padSynthesizer.triggerMinorChord(60, 0.8f); };
    cMinorButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
    cMinorButton.setColour(juce::TextButton::textColourOffId, Theme::text);
    cMinorButton.setAccessible(true);
    playPanel.addAndMakeVisible(cMinorButton);
    
    cSus2Button.setButtonText("C Sus2");
    cSus2Button.setSize(120, 50);
    cSus2Button.onClick = [this] { padSynthesizer.triggerSus2Chord(60, 0.8f); };
    cSus2Button.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
    cSus2Button.setColour(juce::TextButton::textColourOffId, Theme::text);
    cSus2Button.setAccessible(true);
    playPanel.addAndMakeVisible(cSus2Button);
    
    cSus4Button.setButtonText("C Sus4");
    cSus4Button.setSize(120, 50);
    cSus4Button.onClick = [this] { padSynthesizer.triggerSus4Chord(60, 0.8f); };
    cSus4Button.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
    cSus4Button.setColour(juce::TextButton::textColourOffId, Theme::text);
    cSus4Button.setAccessible(true);
    playPanel.addAndMakeVisible(cSus4Button);
}

void MainComponent::setupAdvancedControls()
{
    // Set up play panel container
    addAndMakeVisible(playPanel);
    
    // Set up divider component
    playPanel.addAndMakeVisible(dividerComponent);
    
    // Set up advanced controls toggle with theme styling
    advancedControlsToggle.setButtonText("Advanced Controls");
    advancedControlsToggle.setSize(200, 44); // Minimum 44pt for proper hit targets
        advancedControlsToggle.onClick = [this] { 
            showAdvanced = advancedControlsToggle.getToggleState();
            playPanel.setVisible(!showAdvanced);
            advancedVP.setVisible(showAdvanced);
            
            // Hide divider when showing advanced controls
            dividerComponent.setVisible(!showAdvanced);
            if (showAdvanced) dividerComponent.setBounds(0,0,0,0);
            
            if (showAdvanced) advancedVP.toFront(false); // above playPanel
            
            // Sanity check - should never see both panels true
            juce::Logger::writeToLog(juce::String("Advanced=") + (showAdvanced ? "on" : "off") +
                 "  playPanel vis=" + (playPanel.isVisible() ? "1" : "0") +
                 "  advancedVP vis=" + (advancedVP.isVisible() ? "1" : "0"));
            
            resized(); // relayout the active panel only
        };
    advancedControlsToggle.setColour(juce::ToggleButton::tickColourId, Theme::accent);
    advancedControlsToggle.setColour(juce::ToggleButton::tickDisabledColourId, Theme::textDim);
    advancedControlsToggle.setAccessible(true);
    addAndMakeVisible(advancedControlsToggle);
    
    // Initialize advanced panel viewport
    addAndMakeVisible(advancedVP);
    advancedVP.setViewedComponent(&advancedPanel, false);
    advancedVP.setScrollBarsShown(false, false); // hide both scrollbars completely
    advancedVP.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::transparentBlack);
    advancedVP.getVerticalScrollBar().setColour(juce::ScrollBar::trackColourId, juce::Colours::transparentBlack);
    advancedVP.setVisible(false); // default hidden
    
    // Create sliders with synth parameters
    auto make = [&](int i, const juce::String& name) {
        advancedPanel.labels[i] = std::make_unique<juce::Label>();
        advancedPanel.sliders[i] = std::make_unique<juce::Slider>();
        auto& s = *advancedPanel.sliders[i];
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        s.onValueChange = [this] { updateAdvancedControls(); };
        advancedPanel.addAndMakeVisible(s);
    };
    
    make(0, "Detune"); make(1, "Osc Mix"); make(2, "Attack"); make(3, "Decay");
    make(4, "Sustain"); make(5, "Release"); make(6, "Filter Cutoff"); make(7, "Resonance");
    make(8, "Reverb Size"); make(9, "Reverb Damp"); make(10, "Reverb Wet"); make(11, "Reverb Dry");
    
    // Set up slider ranges and values
    advancedPanel.sliders[0]->setRange(0.0, 2.0, 0.01); advancedPanel.sliders[0]->setValue(0.1); // Detune
    advancedPanel.sliders[1]->setRange(1, 8, 1); advancedPanel.sliders[1]->setValue(4); // Osc Mix
    advancedPanel.sliders[2]->setRange(0.001, 2.0, 0.001); advancedPanel.sliders[2]->setValue(0.1); // Attack
    advancedPanel.sliders[3]->setRange(0.001, 2.0, 0.001); advancedPanel.sliders[3]->setValue(0.3); // Decay
    advancedPanel.sliders[4]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[4]->setValue(0.7); // Sustain
    advancedPanel.sliders[5]->setRange(0.001, 5.0, 0.001); advancedPanel.sliders[5]->setValue(2.0); // Release
    advancedPanel.sliders[6]->setRange(100.0, 8000.0, 1.0); advancedPanel.sliders[6]->setValue(1000.0); // Filter Cutoff
    advancedPanel.sliders[7]->setRange(0.1, 2.0, 0.01); advancedPanel.sliders[7]->setValue(0.7); // Resonance
    advancedPanel.sliders[8]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[8]->setValue(0.5); // Reverb Size
    advancedPanel.sliders[9]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[9]->setValue(0.5); // Reverb Damp
    advancedPanel.sliders[10]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[10]->setValue(0.3); // Reverb Wet
    advancedPanel.sliders[11]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[11]->setValue(0.4); // Reverb Dry
    
    // Set up labels
    for (int i = 0; i < 12; ++i) {
        advancedPanel.labels[i]->setText(advancedPanel.sliders[i]->getName(), juce::dontSendNotification);
        advancedPanel.labels[i]->setJustificationType(juce::Justification::centred);
        advancedPanel.labels[i]->setColour(juce::Label::textColourId, Theme::text);
        advancedPanel.addAndMakeVisible(advancedPanel.labels[i].get());
    }
}

void MainComponent::setupPlayStopButton()
{
    // Set up main control buttons layout
    controlFlexBox.flexDirection = juce::FlexBox::Direction::row;
    controlFlexBox.alignItems = juce::FlexBox::AlignItems::center;
    controlFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    controlFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    
    // Play/Stop button with theme styling
    playStopButton.setButtonText("Play");
    playStopButton.setSize(150, 60);
    playStopButton.onClick = [this] { onPlayStopClicked(); };
    playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
    playStopButton.setColour(juce::TextButton::textColourOffId, Theme::text);
    playStopButton.setAccessible(true);
    addAndMakeVisible(playStopButton);
    
    // Release All button with theme styling
    releaseAllButton.setButtonText("Release All");
    releaseAllButton.setSize(150, 60);
    releaseAllButton.onClick = [this] { onReleaseAllClicked(); };
    releaseAllButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
    releaseAllButton.setColour(juce::TextButton::textColourOffId, Theme::text);
    releaseAllButton.setAccessible(true);
    addAndMakeVisible(releaseAllButton);
}

void MainComponent::setupUIFeedback()
{
    // Set up chord display label with theme typography and stabilization
    chordDisplayLabel.setText("No notes selected", juce::dontSendNotification);
    chordDisplayLabel.setFont(Theme::readout(0.5f)); // Responsive scaling
    chordDisplayLabel.setColour(juce::Label::textColourId, Theme::text);
    chordDisplayLabel.setJustificationType(juce::Justification::centred);
    chordDisplayLabel.setMinimumHorizontalScale(0.8f); // Allow text to shrink to prevent jumping
    playPanel.addAndMakeVisible(chordDisplayLabel);
    
    // Set up playback status label with theme typography and stabilization
    playbackStatusLabel.setText("Stopped", juce::dontSendNotification);
    playbackStatusLabel.setFont(Theme::label());
    playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusStopped);
    playbackStatusLabel.setJustificationType(juce::Justification::centred);
    playbackStatusLabel.setMinimumHorizontalScale(0.8f); // Allow text to shrink to prevent jumping
    playPanel.addAndMakeVisible(playbackStatusLabel);
    
    // Set up active notes label with theme typography and stabilization
    activeNotesLabel.setText("", juce::dontSendNotification);
    activeNotesLabel.setFont(Theme::label(0.85f)); // Slightly smaller
    activeNotesLabel.setColour(juce::Label::textColourId, Theme::accent);
    activeNotesLabel.setJustificationType(juce::Justification::centred);
    activeNotesLabel.setMinimumHorizontalScale(0.8f); // Allow text to shrink to prevent jumping
    playPanel.addAndMakeVisible(activeNotesLabel);
}

void MainComponent::setupFrequencyControls()
{
    // Set up frequency readout with theme typography
    freqReadout.setText("440.0 Hz", juce::dontSendNotification);
    freqReadout.setFont(Theme::readout());
    freqReadout.setColour(juce::Label::textColourId, Theme::text);
    freqReadout.setJustificationType(juce::Justification::centred);
    playPanel.addAndMakeVisible(freqReadout);
    
    // Set up swipe instruction label
    subLabel.setText("Swipe LR: +/-10 Hz  |  UD: +/-1 Hz", juce::dontSendNotification);
    subLabel.setFont(Theme::label());
    subLabel.setColour(juce::Label::textColourId, Theme::textDim);
    subLabel.setJustificationType(juce::Justification::centred);
    playPanel.addAndMakeVisible(subLabel);
}

void MainComponent::setupWaveformControls()
{
    // Create waveform segment buttons
    for (int i = 0; i < waves.size(); ++i)
    {
        auto* b = waveBtns.add(new juce::TextButton(waves[i]));
        b->setClickingTogglesState(true);
        b->onClick = [this, i]() { setWave(i); };
        b->setColour(juce::TextButton::buttonColourId, Theme::btnOff);
        b->setColour(juce::TextButton::textColourOffId, Theme::text);
        b->setColour(juce::TextButton::buttonOnColourId, Theme::btnOn);
        b->setColour(juce::TextButton::textColourOnId, Theme::bg);
        playPanel.addAndMakeVisible(b);
    }
    setWave(0); // Set initial waveform
}

void MainComponent::setupVolumeControls()
{
    // Set up volume label
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setFont(Theme::label());
    volLabel.setColour(juce::Label::textColourId, Theme::textDim);
    volLabel.setJustificationType(juce::Justification::centred);
    volLabel.setMinimumHorizontalScale(0.6f); // allow shrinking to prevent truncation
    playPanel.addAndMakeVisible(volLabel);
    
    // Set up volume sliders
    auto setupSlider = [](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setRange(0.0, 1.0, 0.0);
        s.setValue(0.8);
    };
    
    setupSlider(masterSlider);
    setupSlider(leftSlider);
    setupSlider(rightSlider);
    
    // Add value change callbacks for state persistence
    masterSlider.onValueChange = [this] { saveState(); };
    leftSlider.onValueChange = [this] { saveState(); };
    rightSlider.onValueChange = [this] { saveState(); };
    
    // VoiceOver accessibility for volume sliders
    masterSlider.setAccessible(true);
    leftSlider.setAccessible(true);
    rightSlider.setAccessible(true);
    
    playPanel.addAndMakeVisible(masterSlider);
    playPanel.addAndMakeVisible(leftSlider);
    playPanel.addAndMakeVisible(rightSlider);
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    DBG("=== prepareToPlay called ===");
    DBG("Sample Rate: " << sampleRate << " Hz");
    DBG("Block Size: " << samplesPerBlockExpected);
    DBG("isAudioInitialized before: " << (isAudioInitialized ? "true" : "false"));
    
    // Check if we have a valid sample rate (not 0 Hz from iOS Simulator)
    if (sampleRate <= 0.0)
    {
        DBG("Invalid sample rate detected in prepareToPlay: " << sampleRate << " Hz");

        // Use a fallback sample rate for iOS Simulator
        sampleRate = 48000.0; // Use 48 kHz as fallback (matches iOS reality)
        DBG("Using fallback sample rate: " << sampleRate << " Hz");

        // Show a warning to the user
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Audio Warning",
            "iOS Simulator audio issue detected.\n\n"
            "The app will use a fallback sample rate (48000 Hz).\n"
            "For best results:\n"
            "1. Go to I/O → Audio Output in the Simulator\n"
            "2. Select a real device (e.g., 'MacBook Speakers')\n"
            "3. Restart the app\n\n"
            "This issue rarely occurs on real devices.",
            "OK"
        );
    }
    else
    {
        // Use the actual sample rate provided by the device
        DBG("Using device-provided sample rate: " << sampleRate << " Hz (no fallback needed)");
    }
    
    // Validate sample rate is reasonable
    if (sampleRate < 8000.0 || sampleRate > 192000.0)
    {
        DBG("Sample rate out of reasonable range: " << sampleRate << " Hz, using 48000 Hz fallback");
        sampleRate = 48000.0; // Use 48 kHz fallback (matches iOS reality)
    }
    
    // Validate block size is reasonable
    if (samplesPerBlockExpected <= 0 || samplesPerBlockExpected > 4096)
    {
        DBG("Block size out of reasonable range: " << samplesPerBlockExpected << ", using 512 fallback");
        samplesPerBlockExpected = 512;
    }
    
    try
    {
        // JUCE Synthesiser setup (do this once)
        padSynthesizer.clearVoices();
        for (int i = 0; i < 8; ++i)
            padSynthesizer.addVoice(new PadVoice());   // your real voice class

        padSynthesizer.clearSounds();
        padSynthesizer.addSound(new PadSound());

        padSynthesizer.setCurrentPlaybackSampleRate(sampleRate);
        
        // CRITICAL: Prepare the synthesizer with sample rate and block size
        padSynthesizer.prepare(sampleRate, samplesPerBlockExpected);
        DBG("PadSynthesizer prepared with sample rate: " << sampleRate << " Hz, block size: " << samplesPerBlockExpected);
        
        // Reverb / DSP setup
        juce::dsp::ProcessSpec spec { sampleRate,
                                      (juce::uint32) juce::jlimit(32, 4096, samplesPerBlockExpected),
                                      2 };
        reverb.reset();
        reverb.prepare(spec);
        reverb.setParameters(reverbParams);
        
        isAudioInitialized = true;
        DBG("=== Audio prepared successfully ===");
        DBG("Sample rate: " << sampleRate << " Hz");
        DBG("Block size: " << samplesPerBlockExpected);
        DBG("isAudioInitialized after: " << (isAudioInitialized ? "true" : "false"));
    }
    catch (const std::exception& e)
    {
        DBG("=== Exception in prepareToPlay ===");
        DBG("Error: " << e.what());
        isAudioInitialized = false;
        
        // Show error to user
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Audio Preparation Failed",
            "Failed to prepare audio system.\n\n"
            "Error: " + juce::String(e.what()) + "\n\n"
            "The app will continue without audio functionality.",
            "OK"
        );
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Safety check - if audio isn't initialized, just clear the buffer
    if (!isAudioInitialized || bufferToFill.buffer == nullptr || bufferToFill.numSamples == 0)
    {
        static bool loggedError = false;
        if (!loggedError)
        {
            DBG("ERROR: getNextAudioBlock called but audio not initialized or invalid buffer");
            DBG("  - isAudioInitialized: " << (isAudioInitialized ? "true" : "false"));
            DBG("  - buffer: " << (bufferToFill.buffer ? "valid" : "null"));
            DBG("  - numSamples: " << bufferToFill.numSamples);
            loggedError = true;
        }
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    // Clear first, then generate audio
    bufferToFill.clearActiveBufferRegion();
    
    try
    {
        if (testToneEnabled)
        {
            // Generate simple test tone for debugging
            static double phase = 0.0;
            const double frequency = 440.0;
            const double sampleRate = deviceManager.getCurrentAudioDevice() ? 
                deviceManager.getCurrentAudioDevice()->getCurrentSampleRate() : 44100.0;
            
            for (int i = 0; i < bufferToFill.numSamples; ++i)
            {
                float sample = std::sin(phase) * 0.1f; // Low volume test tone
                phase += 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
                if (phase >= 2.0 * juce::MathConstants<double>::pi)
                    phase -= 2.0 * juce::MathConstants<double>::pi;
                
                // Write to all channels
                for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
                {
                    bufferToFill.buffer->setSample(channel, bufferToFill.startSample + i, sample);
                }
            }
            
            // Debug: Log test tone activity
            static int testToneCounter = 0;
            if (++testToneCounter % 1000 == 0) // Every 1000 audio blocks
            {
                DBG("Test tone active - Sample rate: " << sampleRate 
                    << " Hz, Block size: " << bufferToFill.numSamples);
            }
        }
        else
        {
            // Generate synthesizer audio
            juce::MidiBuffer emptyMidiBuffer;
            padSynthesizer.renderNextBlock(*bufferToFill.buffer, emptyMidiBuffer, bufferToFill.startSample, bufferToFill.numSamples);
            
            // Apply master gain from volume slider
            const float masterGain = juce::jlimit(0.0f, 1.0f, static_cast<float>(masterSlider.getValue()));
            if (masterGain != 1.0f)
            {
                bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, masterGain);
            }
            
            // Apply reverb to the entire mix (after gain)
            juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            reverb.process(context);
            
            // Debug: Log audio activity occasionally
            static int debugCounter = 0;
            if (++debugCounter % 1000 == 0) // Every 1000 audio blocks
            {
                const double currentSampleRate = deviceManager.getCurrentAudioDevice() ? 
                    deviceManager.getCurrentAudioDevice()->getCurrentSampleRate() : 44100.0;
                DBG("Audio rendering active - Sample rate: " << currentSampleRate 
                    << " Hz, Block size: " << bufferToFill.numSamples 
                    << ", Master gain: " << masterGain);
            }
        }
    }
    catch (const std::exception& e)
    {
        DBG("Exception in getNextAudioBlock: " << e.what());
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::releaseResources()
{
    padSynthesizer.allNotesOff(1, true); // Channel 1, allow tail off for graceful release
}

void MainComponent::paint(juce::Graphics& g)
{
    // Use theme background color
    g.fillAll(Theme::bg);

    // Only draw card background when not showing advanced controls
    if (!showAdvanced)
    {
    // Card background for center content - apply safe-area padding
    auto b = getLocalBounds();
    auto sa = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->safeAreaInsets;
    b = b.withTrimmedTop(sa.getTop()).withTrimmedBottom(sa.getBottom())
         .withTrimmedLeft(sa.getLeft()).withTrimmedRight(sa.getRight());
    b = b.reduced(12);
    
    auto center = b.withTrimmedTop(90).withTrimmedBottom(Theme::playHeight + 64);
    g.setColour(Theme::card);
    g.fillRoundedRectangle(center.toFloat(), Theme::corner);
    }
}

void MainComponent::resized()
{
    auto root = getLocalBounds();
    
    // Apply safe-area padding for notch/rounded corners
    auto sa = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->safeAreaInsets;
    root = root.withTrimmedTop(sa.getTop()).withTrimmedBottom(sa.getBottom())
               .withTrimmedLeft(sa.getLeft()).withTrimmedRight(sa.getRight());

    const int gap = 12;
    const int playH = 64;
    
    // Pin play button to bottom after safe-area
    auto playBar = root.removeFromBottom(playH);
    playStopButton.setBounds(playBar.reduced(gap));

    // Header elements (always visible at top)
    auto titleArea = root.removeFromTop(48);
    auto freqArea = root.removeFromTop(64);
    auto subArea = root.removeFromTop(22);
    root.removeFromTop(8);

    // Advanced controls toggle (always visible)
    auto toggleArea = root.removeFromTop(44);
    advancedControlsToggle.setBounds(toggleArea.withSizeKeepingCentre(200, 44));
    
    // The remainder is content for playPanel OR advancedVP
    auto content = root.reduced(gap);

    if (showAdvanced)
    {
        // Single-panel mode: hide play panel completely
        playPanel.setVisible(false);
        playPanel.setBounds(0,0,0,0);            // prevents painting/hits

        // Show and layout advanced panel
        advancedVP.setVisible(true);
        advancedVP.setBounds(content);

        // Give the inner content a fixed larger height to allow scrolling
        const int gap = 12;
        const int rows = 4, cols = 3;
        const int rowH = 100; // slider column height; adjust to taste
        const int innerH = rows * rowH + (rows - 1) * gap + 24;

        advancedPanel.setSize(content.getWidth(), innerH);
        
        // Debug logging for verification
        juce::Logger::writeToLog("Advanced Controls ON: playPanel vis=" + juce::String((int)playPanel.isVisible()) 
            + ", advancedVP vis=" + juce::String((int)advancedVP.isVisible()));
    }
    else
    {
        // Single-panel mode: hide advanced panel completely
        advancedVP.setVisible(false);
        advancedVP.setBounds(0,0,0,0);           // prevents painting/hits

        // Show and layout play panel
        playPanel.setVisible(true);
        playPanel.setBounds(content);

        // Move note/chord layout code into helper that takes a Rectangle
        // and call it here, using playPanel.getLocalBounds() as the base area.
        auto panelArea = playPanel.getLocalBounds();

        layoutNotesAndChords(panelArea); // grid code goes here
        
        // Debug logging for verification
        juce::Logger::writeToLog("Advanced Controls OFF: playPanel vis=" + juce::String((int)playPanel.isVisible()) 
            + ", advancedVP vis=" + juce::String((int)advancedVP.isVisible()));
    }
    
    // Update responsive layout parameters
    updateResponsiveLayout();
}

void MainComponent::layoutNotesAndChords(juce::Rectangle<int> bounds)
{
    // Middle card area
    auto mid = bounds.withTrimmedBottom(Theme::playHeight + 64);

    // Wave segments (top of the card)
    juce::Grid seg;
    seg.autoColumns = juce::Grid::TrackInfo(juce::Grid::Fr(1));
    seg.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)),
                            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
                            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
                            juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
    seg.rowGap = juce::Grid::Px(Theme::gridGap); 
    seg.columnGap = juce::Grid::Px(Theme::gridGap);

    juce::Array<juce::GridItem> items;
    for (auto* btt : waveBtns) { 
        btt->setBounds({}); 
        items.add(juce::GridItem(*btt)); 
    }
    seg.items = items;
    auto segArea = mid.removeFromTop(Theme::segHeight);
    seg.performLayout(segArea.reduced(16));

    mid.removeFromTop(20);

    // Volume area (Master, L, R)
    auto volArea = mid.removeFromTop(static_cast<int>(mid.getHeight() * 0.65f)).reduced(24);
    auto thirdW = volArea.getWidth() / 3;
    masterSlider.setBounds(volArea.removeFromLeft(thirdW).reduced(thirdW/3, 8));
    leftSlider.setBounds(volArea.removeFromLeft(thirdW).reduced(thirdW/3, 8));
    rightSlider.setBounds(volArea.reduced(thirdW/3, 8));
    volLabel.setBounds(mid.removeFromTop(24));

    // ---- NOTE GRID ----
    auto noteArea = bounds.removeFromTop(juce::roundToInt(bounds.getHeight() * 0.42f));

    const int minTap = 44;
    const int gap = juce::roundToInt(Theme::gridGap * 1.5f);

    // Compute columns from width to prevent odd columns
    int cols = juce::jlimit(3, 6, (noteArea.getWidth() + gap) / (minTap + gap));
    int rows = (12 + cols - 1) / cols;
    int tile = juce::jmax(minTap, (noteArea.getWidth() - (cols - 1) * gap) / cols);

    juce::Grid noteGrid;
    noteGrid.autoFlow = juce::Grid::AutoFlow::row;
    noteGrid.alignContent = juce::Grid::AlignContent::center;
    noteGrid.justifyContent = juce::Grid::JustifyContent::center;
    noteGrid.alignItems = juce::Grid::AlignItems::stretch;
    noteGrid.justifyItems = juce::Grid::JustifyItems::stretch;

    // Use fixed Px tracks; don't set .withWidth/.withHeight on items
    noteGrid.templateColumns.clear();
    for (int c = 0; c < cols; ++c)
        noteGrid.templateColumns.add(juce::Grid::TrackInfo(juce::Grid::Px(tile)));

    noteGrid.templateRows.clear();
    for (int r = 0; r < rows; ++r)
        noteGrid.templateRows.add(juce::Grid::TrackInfo(juce::Grid::Px(tile)));

    noteGrid.rowGap = noteGrid.columnGap = juce::Grid::Px(gap);

    noteGrid.items.clear();
    for (auto& b : noteButtons)
        noteGrid.items.add(juce::GridItem(b));

    noteGrid.performLayout(noteArea.withHeight(rows * tile + (rows - 1) * gap));
    
    // ---- CHORD GRID ----
    auto chordArea = bounds.removeFromTop(juce::roundToInt(tile * 2.0f + gap * 1.5f)); // about two rows tall
    
    // Apply card padding and grid inset to prevent clipping into card edge
    auto card = chordArea.reduced(Theme::cardPadding); // Inner padding for card content
    auto gridArea = card.reduced(Theme::gridInset);    // Additional inset for grid layouts

    const int chordMin = 56; // comfortable tap target
    int chordCols = juce::jlimit(2, 4, (gridArea.getWidth() + gap) / (chordMin + gap));
    int chordRows = (4 + chordCols - 1) / chordCols;

    int chordW = juce::jmax(chordMin, (gridArea.getWidth() - (chordCols - 1) * gap) / chordCols);
    int chordH = juce::jmax(chordMin, juce::roundToInt(chordW * 0.75f));

    juce::Grid chord;
    chord.autoFlow = juce::Grid::AutoFlow::row;
    chord.alignContent = juce::Grid::AlignContent::center;
    chord.justifyContent = juce::Grid::JustifyContent::center;
    chord.alignItems = juce::Grid::AlignItems::stretch;
    chord.justifyItems = juce::Grid::JustifyItems::stretch;

    // tracks: fixed Px(chordW) and Px(chordH) so there are no leftover gaps
    chord.templateColumns.clear();
    for (int c = 0; c < chordCols; ++c)
        chord.templateColumns.add(juce::Grid::TrackInfo(juce::Grid::Px(chordW)));

    chord.templateRows.clear();
    for (int r = 0; r < chordRows; ++r)
        chord.templateRows.add(juce::Grid::TrackInfo(juce::Grid::Px(chordH)));

    chord.rowGap = juce::Grid::Px(gap);
    chord.columnGap = juce::Grid::Px(gap);

    chord.items.clear();
    chord.items.add(juce::GridItem(cMajorButton)); // No withWidth/withHeight - let tracks size items
    chord.items.add(juce::GridItem(cMinorButton));
    chord.items.add(juce::GridItem(cSus2Button));
    chord.items.add(juce::GridItem(cSus4Button));

    // Use grid area with proper height calculation to prevent overflow
    chord.performLayout(gridArea.withHeight(chordRows * chordH + (chordRows - 1) * gap));
    
    // Divider component
    auto dividerArea = bounds.removeFromTop(1);
    dividerComponent.setBounds(dividerArea);
}

void MainComponent::onNoteButtonClicked(int noteIndex)
{
    // Add haptic feedback on iOS
    triggerHapticFeedback();
    
    // Audio is now initialized in constructor, just check if it's ready
    if (!isAudioInitialized)
    {
        // Audio initialization failed - user was already notified
        return;
    }
    
    // Hook the UI to MIDI note numbers and velocities that aren't zero
    const int midiNote = 60 + noteIndex;   // C4 + offset
    const bool on = noteButtons[noteIndex].getToggleState();

    if (on)  
        padSynthesizer.noteOn(1, midiNote, 0.9f);
    else     
        padSynthesizer.noteOff(1, midiNote, 0.9f, true);
    
    // If we're not currently playing, start playing automatically
    if (on && !isPlaying)
    {
        isPlaying = true;
        playStopButton.setButtonText("Stop");
        playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnActive);
        playbackStatusLabel.setText("Playing", juce::dontSendNotification);
        playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusPlaying);
    }
    else if (!on)
    {
        // Check if any notes are still selected
        bool anyNotesSelected = false;
        for (int i = 0; i < 12; ++i)
        {
            if (noteButtons[i].getToggleState())
            {
                anyNotesSelected = true;
                break;
            }
        }
        
        // If no notes are selected and we're playing, stop playback
        if (!anyNotesSelected && isPlaying)
        {
            isPlaying = false;
            playStopButton.setButtonText("Play");
            playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
            playbackStatusLabel.setText("Stopped", juce::dontSendNotification);
            playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusStopped);
        }
    }
    
    // Update UI feedback
    updateChordDisplay();
    updateNoteButtonVisualFeedback();
    updateActiveNotesDisplay();
    
    // Save state
    saveState();
}

void MainComponent::onChordButtonClicked(const std::vector<int>& chord)
{
    // Add haptic feedback on iOS
    triggerHapticFeedback();
    
    padSynthesizer.triggerChord(chord, 1.0f);
}

void MainComponent::onPlayStopClicked()
{
    // Add haptic feedback on iOS
    triggerHapticFeedback();
    
    // Audio is now initialized in constructor, just check if it's ready
    if (!isAudioInitialized)
    {
        // Audio initialization failed - user was already notified
        return;
    }
    
    if (isPlaying)
    {
        // Stop all notes gracefully using allNotesOff()
        padSynthesizer.allNotesOff(1, true); // Channel 1, allow tail off for graceful release
        playStopButton.setButtonText("Play");
        playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
        isPlaying = false;
        playbackStatusLabel.setText("Stopped", juce::dontSendNotification);
        playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusStopped);
        
        // Note: We don't reset note button toggles here - let them stay selected
        // so user can see what was playing and potentially play again
    }
    else
    {
        // Start playing - iterate through all note toggles and start selected notes
        bool anyNotesSelected = false;
        
        for (int i = 0; i < 12; ++i)
        {
            if (noteButtons[i].getToggleState())
            {
                int midiNote = 60 + i; // C4 + semitone offset
                padSynthesizer.noteOn(1, midiNote, 0.8f); // Channel 1, velocity 0.8
                anyNotesSelected = true;
            }
        }
        
        // If no notes are selected, do nothing (don't play default)
        if (anyNotesSelected)
        {
            playStopButton.setButtonText("Stop");
            playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnActive);
            isPlaying = true;
            playbackStatusLabel.setText("Playing", juce::dontSendNotification);
            playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusPlaying);
        }
        // If no notes selected, button remains in "Play" state
    }
    
    // Update UI feedback
    updateNoteButtonVisualFeedback();
    updateActiveNotesDisplay();
}

void MainComponent::onReleaseAllClicked()
{
    // Stop all notes gracefully using allNotesOff()
    padSynthesizer.allNotesOff(1, true); // Channel 1, allow tail off for graceful release
    
    // Reset all note buttons
    for (auto& button : noteButtons)
    {
        button.setToggleState(false, juce::dontSendNotification);
    }
    
    // Reset play button
    if (isPlaying)
    {
        playStopButton.setButtonText("Play");
        playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
        isPlaying = false;
    }
    
    // Update UI feedback
    updateChordDisplay();
    updateNoteButtonVisualFeedback();
    updateActiveNotesDisplay();
    playbackStatusLabel.setText("Stopped", juce::dontSendNotification);
    playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusStopped);
}

void MainComponent::onAdvancedControlsToggle()
{
    showAdvanced = advancedControlsToggle.getToggleState();
    playPanel.setVisible(!showAdvanced);
    advancedVP.setVisible(showAdvanced);
    
    // Hide divider when showing advanced controls
    dividerComponent.setVisible(!showAdvanced);
    if (showAdvanced) dividerComponent.setBounds(0,0,0,0);
    
    if (showAdvanced) advancedVP.toFront(false); // above playPanel
    
    // Sanity check - should never see both panels true
    DBG (juce::String("Advanced=") + (showAdvanced ? "on" : "off") +
         "  playPanel vis=" + (playPanel.isVisible() ? "1" : "0") +
         "  advancedVP vis=" + (advancedVP.isVisible() ? "1" : "0"));
    
    // Resize window based on advanced controls visibility - responsive sizing
    if (showAdvanced)
    {
        setSize(1400, 1200); // Much larger window for advanced controls
    }
    else
    {
        setSize(1000, 800); // Larger window for basic controls with better spacing
    }
    
    // Layout will be updated automatically when the window resizes
    
    // Save state
    saveState();
}

void MainComponent::setFrequency(double hz)
{
    hz = juce::jlimit(20.0, 22000.0, hz);
    frequencyHz = hz;
    freqReadout.setText(juce::String(hz, 1) + " Hz", juce::dontSendNotification);
    
    // Update synthesizer frequency
    padSynthesizer.setGlobalFrequency(hz);
    
    // Save state
    saveState();
}

void MainComponent::setWave(int idx)
{
    // Add haptic feedback on iOS
    triggerHapticFeedback();
    
    waveIndex = juce::jlimit(0, waves.size() - 1, idx);
    for (int i = 0; i < waveBtns.size(); ++i)
        waveBtns[i]->setToggleState(i == waveIndex, juce::dontSendNotification);
    
    // Update synthesizer waveform
    padSynthesizer.setGlobalWaveform(waveIndex);
    
    // Save state
    saveState();
}

void MainComponent::updateAdvancedControls()
{
    if (advancedPanel.sliders[0]) padSynthesizer.setDetuneAmount(static_cast<float>(advancedPanel.sliders[0]->getValue()));
    if (advancedPanel.sliders[1]) padSynthesizer.setOscillatorCount(static_cast<int>(advancedPanel.sliders[1]->getValue()));
    if (advancedPanel.sliders[2]) padSynthesizer.setEnvelopeAttack(static_cast<float>(advancedPanel.sliders[2]->getValue()));
    if (advancedPanel.sliders[3]) padSynthesizer.setEnvelopeDecay(static_cast<float>(advancedPanel.sliders[3]->getValue()));
    if (advancedPanel.sliders[4]) padSynthesizer.setEnvelopeSustain(static_cast<float>(advancedPanel.sliders[4]->getValue()));
    if (advancedPanel.sliders[5]) padSynthesizer.setEnvelopeRelease(static_cast<float>(advancedPanel.sliders[5]->getValue()));
    if (advancedPanel.sliders[6]) padSynthesizer.setFilterCutoff(static_cast<float>(advancedPanel.sliders[6]->getValue()));
    if (advancedPanel.sliders[7]) padSynthesizer.setFilterResonance(static_cast<float>(advancedPanel.sliders[7]->getValue()));
    
    if (advancedPanel.sliders[8]) reverbParams.roomSize = static_cast<float>(advancedPanel.sliders[8]->getValue());
    if (advancedPanel.sliders[9]) reverbParams.damping = static_cast<float>(advancedPanel.sliders[9]->getValue());
    if (advancedPanel.sliders[10]) reverbParams.wetLevel = static_cast<float>(advancedPanel.sliders[10]->getValue());
    if (advancedPanel.sliders[11]) reverbParams.dryLevel = static_cast<float>(advancedPanel.sliders[11]->getValue());
    reverb.setParameters(reverbParams);
    
    // Save state when advanced controls change
    saveState();
}

void MainComponent::updateChordDisplay()
{
    // Get currently selected notes
    std::vector<int> selectedNotes;
    for (int i = 0; i < 12; ++i)
    {
        if (noteButtons[i].getToggleState())
        {
            selectedNotes.push_back(60 + i); // C4 + semitone offset
        }
    }
    
    if (selectedNotes.empty())
    {
        chordDisplayLabel.setText("No notes selected", juce::dontSendNotification);
    }
    else
    {
        juce::String chordName = getChordName(selectedNotes);
        if (isPlaying)
        {
            chordDisplayLabel.setText("Currently playing: " + chordName, juce::dontSendNotification);
        }
        else
        {
            chordDisplayLabel.setText("Selected: " + chordName, juce::dontSendNotification);
        }
    }
}

void MainComponent::updateNoteButtonVisualFeedback()
{
    // Update visual feedback for note buttons based on playback state
    for (int i = 0; i < 12; ++i)
    {
        bool isSelected = noteButtons[i].getToggleState();
        bool isCurrentlyPlaying = isPlaying && isSelected;
        
        if (isCurrentlyPlaying)
        {
            // Note is actively sounding - use theme playing color
            noteButtons[i].setColour(juce::TextButton::buttonOnColourId, Theme::notePlaying);
            noteButtons[i].setColour(juce::TextButton::textColourOnId, juce::Colour(0xff000000));
            // Add a subtle glow effect by using a brighter border
            noteButtons[i].setColour(juce::TextButton::buttonColourId, Theme::notePlaying.brighter(0.2f));
        }
        else if (isSelected)
        {
            // Note is selected but not playing - use theme selected color
            noteButtons[i].setColour(juce::TextButton::buttonOnColourId, Theme::noteSelected);
            noteButtons[i].setColour(juce::TextButton::textColourOnId, juce::Colour(0xff000000));
        }
        else
        {
            // Note is not selected - use theme off color
            noteButtons[i].setColour(juce::TextButton::buttonColourId, Theme::noteOff);
            noteButtons[i].setColour(juce::TextButton::textColourOffId, Theme::text);
        }
    }
}

juce::String MainComponent::getChordName(const std::vector<int>& activeNotes)
{
    if (activeNotes.empty())
        return "No notes";
    
    if (activeNotes.size() == 1)
    {
        int noteIndex = (activeNotes[0] - 60) % 12;
        return noteNames[noteIndex];
    }
    
    // Simple chord recognition for common chords
    std::vector<int> noteClasses;
    for (int note : activeNotes)
    {
        noteClasses.push_back((note - 60) % 12);
    }
    std::sort(noteClasses.begin(), noteClasses.end());
    
    // Check for major chord (root, major third, perfect fifth)
    if (noteClasses.size() >= 3)
    {
        for (int root = 0; root < 12; ++root)
        {
            int majorThird = (root + 4) % 12;
            int perfectFifth = (root + 7) % 12;
            
            if (std::find(noteClasses.begin(), noteClasses.end(), root) != noteClasses.end() &&
                std::find(noteClasses.begin(), noteClasses.end(), majorThird) != noteClasses.end() &&
                std::find(noteClasses.begin(), noteClasses.end(), perfectFifth) != noteClasses.end())
            {
                return noteNames[root] + " Major";
            }
        }
        
        // Check for minor chord (root, minor third, perfect fifth)
        for (int root = 0; root < 12; ++root)
        {
            int minorThird = (root + 3) % 12;
            int perfectFifth = (root + 7) % 12;
            
            if (std::find(noteClasses.begin(), noteClasses.end(), root) != noteClasses.end() &&
                std::find(noteClasses.begin(), noteClasses.end(), minorThird) != noteClasses.end() &&
                std::find(noteClasses.begin(), noteClasses.end(), perfectFifth) != noteClasses.end())
            {
                return noteNames[root] + " Minor";
            }
        }
    }
    
    // If no recognized chord, show the notes
    juce::String result = "";
    for (size_t i = 0; i < noteClasses.size(); ++i)
    {
        if (i > 0) result += " ";
        result += noteNames[noteClasses[i]];
    }
    return result;
}

void MainComponent::updateActiveNotesDisplay()
{
    if (!isPlaying)
    {
        activeNotesLabel.setText("", juce::dontSendNotification);
        return;
    }
    
    // Get currently playing notes
    std::vector<int> playingNotes;
    for (int i = 0; i < 12; ++i)
    {
        if (noteButtons[i].getToggleState())
        {
            playingNotes.push_back(i);
        }
    }
    
    if (playingNotes.empty())
    {
        activeNotesLabel.setText("", juce::dontSendNotification);
    }
    else
    {
        juce::String activeText = "Playing: ";
        for (size_t i = 0; i < playingNotes.size(); ++i)
        {
            if (i > 0) activeText += " ";
            activeText += noteNames[playingNotes[i]];
        }
        activeNotesLabel.setText(activeText, juce::dontSendNotification);
    }
}

// Responsive design helper methods
int MainComponent::calculateButtonSize(int availableWidth) const
{
    // Calculate button size based on available width and number of columns
    int buttonsPerRow = calculateButtonsPerRow(availableWidth);
    int calculatedSize = (availableWidth - (buttonsPerRow + 1) * BUTTON_MARGIN) / buttonsPerRow;
    
    // Clamp to min/max bounds
    return juce::jlimit(MIN_BUTTON_SIZE, MAX_BUTTON_SIZE, calculatedSize);
}

int MainComponent::calculateButtonsPerRow(int availableWidth) const
{
    if (availableWidth < SMALL_PHONE_BREAKPOINT)
        return 4; // 4 buttons per row for small phones
    else if (availableWidth < MEDIUM_PHONE_BREAKPOINT)
        return 5; // 5 buttons per row for medium phones
    else if (availableWidth < TABLET_BREAKPOINT)
        return 6; // 6 buttons per row for large phones/small tablets
    else
        return 6; // 6 buttons per row for tablets and larger
}

bool MainComponent::isTabletSize() const
{
    return getWidth() >= TABLET_BREAKPOINT;
}

bool MainComponent::isSmallPhoneSize() const
{
    return getWidth() < SMALL_PHONE_BREAKPOINT;
}

bool MainComponent::isMediumPhoneSize() const
{
    return getWidth() >= SMALL_PHONE_BREAKPOINT && getWidth() < MEDIUM_PHONE_BREAKPOINT;
}

bool MainComponent::isLargeTabletSize() const
{
    return getWidth() >= LARGE_TABLET_BREAKPOINT;
}

bool MainComponent::isLandscapeOrientation() const
{
    return getWidth() > getHeight();
}

int MainComponent::getResponsiveMargin() const
{
    if (isSmallPhoneSize())
        return 4;
    else if (isMediumPhoneSize())
        return 6;
    else if (isTabletSize())
        return 8;
    else
        return 10;
}

void MainComponent::updateButtonSizes()
{
    int buttonSize = calculateButtonSize(getWidth());
    
    // Update all note buttons
    for (auto& button : noteButtons)
    {
        button.setSize(buttonSize, buttonSize);
    }
    
    // Update chord buttons - responsive sizing
    int chordButtonWidth = juce::jlimit(100, 150, getWidth() / 5);
    int chordButtonHeight = juce::jlimit(40, 60, static_cast<int>(buttonSize * 0.7f));
    
    cMajorButton.setSize(chordButtonWidth, chordButtonHeight);
    cMinorButton.setSize(chordButtonWidth, chordButtonHeight);
    cSus2Button.setSize(chordButtonWidth, chordButtonHeight);
    cSus4Button.setSize(chordButtonWidth, chordButtonHeight);
    
    // Update control buttons - responsive sizing
    int controlButtonWidth = juce::jlimit(120, 200, getWidth() / 4);
    int controlButtonHeight = juce::jlimit(50, 80, buttonSize);
    
    playStopButton.setSize(controlButtonWidth, controlButtonHeight);
    releaseAllButton.setSize(controlButtonWidth, controlButtonHeight);
}

void MainComponent::updateResponsiveLayout()
{
    // Update button sizes
    updateButtonSizes();
    
    // Update font sizes for labels based on screen size and orientation
    float baseFontSize;
    if (isSmallPhoneSize())
        baseFontSize = 14.0f;
    else if (isMediumPhoneSize())
        baseFontSize = 16.0f;
    else if (isTabletSize())
        baseFontSize = isLandscapeOrientation() ? 22.0f : 20.0f;
    else
        baseFontSize = 24.0f;
    
    chordDisplayLabel.setFont(Theme::readout(baseFontSize / 44.0f)); // Scale from base readout size
    playbackStatusLabel.setFont(Theme::label(baseFontSize / 14.0f)); // Scale from base label size
    activeNotesLabel.setFont(Theme::label(baseFontSize * 0.6f / 14.0f)); // Scale from base label size
    
    // Update title font size for responsive design
    // This will be applied in the paint() method
    
    // DON'T call resized() here - it creates infinite recursion!
    // The layout will be updated when the component is actually resized
}

void MainComponent::initialiseAudioSafely()
{
    std::cout << "initialiseAudioSafely() called" << std::endl;
    std::cout.flush();
    
#if JUCE_IOS
    // Quick bring-up (Objective-C++) - proper iOS audio setup
    @autoreleasepool {
        NSError* err = nil;
        AVAudioSession* s = [AVAudioSession sharedInstance];
        
        // Deactivate first to ensure clean state
        [s setActive:NO error:&err];
        if (err != nil) {
            DBG("AVAudioSession deactivation error: " << [[err localizedDescription] UTF8String]);
        }
        
        // Set category to Playback so hardware mute switch doesn't silence us
        [s setCategory:AVAudioSessionCategoryPlayback withOptions:0 error:&err];
        if (err != nil) {
            DBG("AVAudioSession category error: " << [[err localizedDescription] UTF8String]);
        }
        
        // Accept 48 kHz end-to-end; don't coerce to 44.1 later
        [s setPreferredSampleRate:48000 error:&err];
        if (err != nil) {
            DBG("AVAudioSession sample rate error: " << [[err localizedDescription] UTF8String]);
        }
        
        // Low latency buffer duration
        [s setPreferredIOBufferDuration:0.005 error:&err];
        if (err != nil) {
            DBG("AVAudioSession buffer duration error: " << [[err localizedDescription] UTF8String]);
        }
        
        // Activate the session
        [s setActive:YES error:&err];
        if (err != nil) {
            DBG("AVAudioSession activation error: " << [[err localizedDescription] UTF8String]);
        } else {
            const double actualSampleRate = [s sampleRate];
            DBG("AVAudioSession configured successfully - Sample Rate: " << actualSampleRate << " Hz, Channels: " << [s outputNumberOfChannels]);
        }
        
        // Set up route change notifications
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionRouteChangeNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* notification) {
            onAudioRouteChanged();
        }];
        
        // Set up interruption notifications
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* notification) {
            onAudioInterruption((__bridge void*)notification);
        }];
        
        // Call setAudioChannels after AVAudioSession is active, and don't sleep—gate on a valid route
        juce::MessageManager::callAsync([this] {
            DBG("iOS: Calling setAudioChannels(0, 2)");
            setAudioChannels(0, 2);   // opens the device => prepareToPlay will run
        });
    }
#else
    // Non-iOS platforms: set audio channels immediately
    std::cout << "Non-iOS platform: Calling setAudioChannels(0, 2) immediately" << std::endl;
    std::cout.flush();
    setAudioChannels(0, 2);
    
    // Wait a moment for the device to open
    juce::Thread::sleep(100); // Wait 100ms
    
    // Check if audio device opened successfully
    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        DBG("=== Audio device opened successfully ===");
        DBG("Device: " << device->getName());
        DBG("Sample Rate: " << device->getCurrentSampleRate() << " Hz");
        DBG("Buffer Size: " << device->getCurrentBufferSizeSamples() << " samples");
        DBG("Output Channels: " << device->getOutputChannelNames().size());
        
        // Force prepareToPlay to be called
        DBG("=== Manually calling prepareToPlay ===");
        prepareToPlay(device->getCurrentBufferSizeSamples(), device->getCurrentSampleRate());
    }
    else
    {
        DBG("=== ERROR: Failed to open audio device on non-iOS platform ===");
        DBG("=== Attempting to manually open audio device ===");
        
        // Try to manually open the audio device
        juce::AudioDeviceManager::AudioDeviceSetup setup;
        setup.outputDeviceName = ""; // Use default device
        setup.sampleRate = 44100.0;
        setup.bufferSize = 256;
        setup.useDefaultInputChannels = false;
        setup.useDefaultOutputChannels = true;
        setup.inputChannels = 0;
        setup.outputChannels = 2;
        
        juce::String error = deviceManager.setAudioDeviceSetup(setup, true);
        if (error.isEmpty())
        {
            DBG("=== Successfully opened audio device manually ===");
            if (auto* device = deviceManager.getCurrentAudioDevice())
            {
                DBG("Device: " << device->getName());
                DBG("Sample Rate: " << device->getCurrentSampleRate() << " Hz");
                DBG("Buffer Size: " << device->getCurrentBufferSizeSamples() << " samples");
                
                // Force prepareToPlay to be called
                DBG("=== Manually calling prepareToPlay after manual device open ===");
                prepareToPlay(device->getCurrentBufferSizeSamples(), device->getCurrentSampleRate());
            }
        }
        else
        {
            DBG("=== Failed to open audio device manually: " << error << " ===");
        }
    }
#endif
}


void MainComponent::onAudioRouteChanged()
{
#if JUCE_IOS
    @autoreleasepool {
        AVAudioSession* session = [AVAudioSession sharedInstance];
        AVAudioSessionRouteDescription* route = [session currentRoute];
        
        // Check if we have a valid output route
        bool hasValidOutput = NO;
        for (AVAudioSessionPortDescription* output in [route outputs]) {
            if ([[output portType] isEqualToString:AVAudioSessionPortBuiltInSpeaker] ||
                [[output portType] isEqualToString:AVAudioSessionPortHeadphones] ||
                [[output portType] isEqualToString:AVAudioSessionPortBluetoothA2DP] ||
                [[output portType] isEqualToString:AVAudioSessionPortBluetoothHFP] ||
                [[output portType] isEqualToString:AVAudioSessionPortAirPlay]) {
                hasValidOutput = YES;
                break;
            }
        }
        
        DBG("Audio route changed - Has valid output: " << (hasValidOutput ? "YES" : "NO"));
        
        if (hasValidOutput && !audioRouteReady) {
            // Route is ready, initialize audio channels
            audioRouteReady = true;
            setAudioChannels(0, 2);
            isAudioInitialized = false;
            DBG("Audio route ready - Audio channels initialized");
        } else if (!hasValidOutput && audioRouteReady) {
            // Route became invalid, mark as not ready
            audioRouteReady = false;
            DBG("Audio route became invalid - Waiting for valid route");
        }
    }
#endif
}

void MainComponent::onAudioInterruption(void* notification)
{
#if JUCE_IOS
    @autoreleasepool {
        NSNotification* nsNotification = (__bridge NSNotification*)notification;
        AVAudioSessionInterruptionType interruptionType = (AVAudioSessionInterruptionType)[nsNotification.userInfo[AVAudioSessionInterruptionTypeKey] integerValue];
        
        if (interruptionType == AVAudioSessionInterruptionTypeBegan)
        {
            // Audio was interrupted (e.g., phone call, Siri, etc.)
            DBG("Audio interruption began - closing audio device");
            
            // Stop all audio and close the device
            padSynthesizer.allNotesOff(1, true); // Graceful release
            deviceManager.closeAudioDevice();
            isAudioInitialized = false;
            
            // Update UI to reflect stopped state
            isPlaying = false;
            playStopButton.setButtonText("Play");
            playStopButton.setColour(juce::TextButton::buttonColourId, Theme::btnOff);
            playbackStatusLabel.setColour(juce::Label::textColourId, Theme::statusStopped);
            playbackStatusLabel.setText("Stopped", juce::dontSendNotification);
        }
        else if (interruptionType == AVAudioSessionInterruptionTypeEnded)
        {
            // Audio interruption ended (e.g., call ended)
            DBG("Audio interruption ended - reopening audio device");
            
            // Check if we should resume audio
            NSNumber* shouldResume = nsNotification.userInfo[AVAudioSessionInterruptionOptionKey];
            if (shouldResume && [shouldResume integerValue] == AVAudioSessionInterruptionOptionShouldResume)
            {
                // Reactivate the audio session
                AVAudioSession* session = [AVAudioSession sharedInstance];
                NSError* error = nil;
                [session setActive:YES error:&error];
                
                if (error)
                {
                    DBG("Failed to reactivate audio session: " << [[error localizedDescription] UTF8String]);
                }
                else
                {
                    DBG("Audio session reactivated successfully");
                    
                    // Reopen audio channels
                    setAudioChannels(0, 2);
                    
                    // Wait a moment for the device to be ready
                    juce::Timer::callAfterDelay(100, [this]() {
                        // Check if audio is now ready
                        if (deviceManager.getCurrentAudioDevice() != nullptr)
                        {
                            isAudioInitialized = true;
                            audioRouteReady = true;
                            DBG("Audio device reopened successfully after interruption");
                        }
                        else
                        {
                            DBG("Failed to reopen audio device after interruption");
                        }
                    });
                }
            }
            else
            {
                DBG("Audio interruption ended but should not resume");
            }
        }
    }
#endif
}

void MainComponent::triggerHapticFeedback()
{
#if JUCE_IOS
    // Check if haptic feedback is available (not available in simulator)
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        // Try multiple haptic feedback approaches for better reliability
        static UIImpactFeedbackGenerator* heavyGenerator = nil;
        static UIImpactFeedbackGenerator* mediumGenerator = nil;
        static UIImpactFeedbackGenerator* lightGenerator = nil;
        
        if (heavyGenerator == nil)
        {
            heavyGenerator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleHeavy];
            mediumGenerator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleMedium];
            lightGenerator = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleLight];
            DBG("Haptic feedback generators initialized for device");
        }
        
        // Try heavy haptic feedback first (most noticeable)
        [heavyGenerator prepare];
        [heavyGenerator impactOccurred];
        
        // Also try a selection feedback for additional tactile response
        UISelectionFeedbackGenerator* selectionGenerator = [[UISelectionFeedbackGenerator alloc] init];
        [selectionGenerator prepare];
        [selectionGenerator selectionChanged];
        
        DBG("Heavy haptic feedback triggered on device");
    }
    else
    {
        DBG("Haptic feedback not available in simulator");
    }
#endif
}

void MainComponent::toggleTestTone()
{
    testToneEnabled = !testToneEnabled;
    DBG("Test tone " << (testToneEnabled ? "enabled" : "disabled"));
    
    // Show user feedback
    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Audio Debug",
        testToneEnabled ? 
            "Test tone enabled - You should hear a 440 Hz sine wave" :
            "Test tone disabled - Normal synthesizer audio restored",
        "OK"
    );
}

void MainComponent::initializePropertiesFile()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "ToneGenerator";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    options.commonToAllUsers = false;
    options.ignoreCaseOfKeyNames = false;
    options.doNotSave = false;
    options.millisecondsBeforeSaving = 3000; // Auto-save after 3 seconds of inactivity
    
    propertiesFile = std::make_unique<juce::PropertiesFile>(options);
}

void MainComponent::saveState()
{
    if (propertiesFile == nullptr) return;
    
    // Save waveform selection
    propertiesFile->setValue("waveform", waveIndex);
    
    // Save volume settings
    propertiesFile->setValue("masterVolume", masterSlider.getValue());
    propertiesFile->setValue("leftVolume", leftSlider.getValue());
    propertiesFile->setValue("rightVolume", rightSlider.getValue());
    
    // Save selected notes
    juce::StringArray selectedNotes;
    for (int i = 0; i < 12; ++i)
    {
        if (noteButtons[i].getToggleState())
        {
            selectedNotes.add(juce::String(i));
        }
    }
    propertiesFile->setValue("selectedNotes", selectedNotes.joinIntoString(","));
    
    // Save frequency
    propertiesFile->setValue("frequency", frequencyHz);
    
    // Save advanced controls visibility
    propertiesFile->setValue("showAdvancedControls", showAdvanced);
    
    // Save advanced control values
    if (advancedPanel.sliders[0]) propertiesFile->setValue("detune", advancedPanel.sliders[0]->getValue());
    if (advancedPanel.sliders[1]) propertiesFile->setValue("oscillatorCount", advancedPanel.sliders[1]->getValue());
    if (advancedPanel.sliders[2]) propertiesFile->setValue("attack", advancedPanel.sliders[2]->getValue());
    if (advancedPanel.sliders[3]) propertiesFile->setValue("decay", advancedPanel.sliders[3]->getValue());
    if (advancedPanel.sliders[4]) propertiesFile->setValue("sustain", advancedPanel.sliders[4]->getValue());
    if (advancedPanel.sliders[5]) propertiesFile->setValue("release", advancedPanel.sliders[5]->getValue());
    if (advancedPanel.sliders[6]) propertiesFile->setValue("filterCutoff", advancedPanel.sliders[6]->getValue());
    if (advancedPanel.sliders[7]) propertiesFile->setValue("filterResonance", advancedPanel.sliders[7]->getValue());
    if (advancedPanel.sliders[8]) propertiesFile->setValue("reverbRoomSize", advancedPanel.sliders[8]->getValue());
    if (advancedPanel.sliders[9]) propertiesFile->setValue("reverbDamping", advancedPanel.sliders[9]->getValue());
    if (advancedPanel.sliders[10]) propertiesFile->setValue("reverbWetLevel", advancedPanel.sliders[10]->getValue());
    if (advancedPanel.sliders[11]) propertiesFile->setValue("reverbDryLevel", advancedPanel.sliders[11]->getValue());
    
    // Save the file
    propertiesFile->saveIfNeeded();
    DBG("State saved successfully");
}

void MainComponent::loadState()
{
    if (propertiesFile == nullptr) return;
    
    // Load waveform selection
    waveIndex = propertiesFile->getIntValue("waveform", 0);
    setWave(waveIndex);
    
    // Load volume settings
    masterSlider.setValue(propertiesFile->getDoubleValue("masterVolume", 0.8));
    leftSlider.setValue(propertiesFile->getDoubleValue("leftVolume", 0.8));
    rightSlider.setValue(propertiesFile->getDoubleValue("rightVolume", 0.8));
    
    // Load selected notes
    juce::String selectedNotesStr = propertiesFile->getValue("selectedNotes", "");
    if (selectedNotesStr.isNotEmpty())
    {
        juce::StringArray selectedNotes;
        selectedNotes.addTokens(selectedNotesStr, ",", "");
        
        // Clear all notes first
        for (int i = 0; i < 12; ++i)
        {
            noteButtons[i].setToggleState(false, juce::dontSendNotification);
        }
        
        // Restore selected notes
        for (const auto& noteStr : selectedNotes)
        {
            int noteIndex = noteStr.getIntValue();
            if (noteIndex >= 0 && noteIndex < 12)
            {
                noteButtons[noteIndex].setToggleState(true, juce::dontSendNotification);
            }
        }
    }
    
    // Load frequency
    frequencyHz = propertiesFile->getDoubleValue("frequency", 440.0);
    setFrequency(frequencyHz);
    
    // Load advanced controls visibility
    showAdvanced = propertiesFile->getBoolValue("showAdvancedControls", false);
    advancedControlsToggle.setToggleState(showAdvanced, juce::dontSendNotification);
    
    // Load advanced control values
    if (advancedPanel.sliders[0]) advancedPanel.sliders[0]->setValue(propertiesFile->getDoubleValue("detune", 0.1));
    if (advancedPanel.sliders[1]) advancedPanel.sliders[1]->setValue(propertiesFile->getDoubleValue("oscillatorCount", 4.0));
    if (advancedPanel.sliders[2]) advancedPanel.sliders[2]->setValue(propertiesFile->getDoubleValue("attack", 0.01));
    if (advancedPanel.sliders[3]) advancedPanel.sliders[3]->setValue(propertiesFile->getDoubleValue("decay", 0.3));
    if (advancedPanel.sliders[4]) advancedPanel.sliders[4]->setValue(propertiesFile->getDoubleValue("sustain", 0.7));
    if (advancedPanel.sliders[5]) advancedPanel.sliders[5]->setValue(propertiesFile->getDoubleValue("release", 2.0));
    if (advancedPanel.sliders[6]) advancedPanel.sliders[6]->setValue(propertiesFile->getDoubleValue("filterCutoff", 2000.0));
    if (advancedPanel.sliders[7]) advancedPanel.sliders[7]->setValue(propertiesFile->getDoubleValue("filterResonance", 0.7));
    if (advancedPanel.sliders[8]) advancedPanel.sliders[8]->setValue(propertiesFile->getDoubleValue("reverbRoomSize", 0.5));
    if (advancedPanel.sliders[9]) advancedPanel.sliders[9]->setValue(propertiesFile->getDoubleValue("reverbDamping", 0.5));
    if (advancedPanel.sliders[10]) advancedPanel.sliders[10]->setValue(propertiesFile->getDoubleValue("reverbWetLevel", 0.3));
    if (advancedPanel.sliders[11]) advancedPanel.sliders[11]->setValue(propertiesFile->getDoubleValue("reverbDryLevel", 0.4));
    
    // Update synthesizer with loaded values
    updateAdvancedControls();
    
    // Update UI feedback
    updateChordDisplay();
    updateNoteButtonVisualFeedback();
    updateActiveNotesDisplay();
    
    DBG("State loaded successfully");
}

void MainComponent::saveSettings()
{
    saveState();
}

void MainComponent::loadSettings()
{
    loadState();
}

void MainComponent::setupAccessibilityFocusOrder()
{
    // Set up focus order: note grid first, then chords, then transport
    // This ensures VoiceOver users can navigate logically through the interface
    
    // Note buttons (0-11) - first priority
    for (int i = 0; i < 12; ++i)
    {
        noteButtons[i].setExplicitFocusOrder(i + 1);
    }
    
    // Chord buttons (12-15) - second priority
    cMajorButton.setExplicitFocusOrder(13);
    cMinorButton.setExplicitFocusOrder(14);
    cSus2Button.setExplicitFocusOrder(15);
    cSus4Button.setExplicitFocusOrder(16);
    
    // Transport controls (17-18) - third priority
    playStopButton.setExplicitFocusOrder(17);
    releaseAllButton.setExplicitFocusOrder(18);
    
    // Waveform controls (19-22) - fourth priority
    for (int i = 0; i < waveBtns.size(); ++i)
    {
        waveBtns[i]->setExplicitFocusOrder(19 + i);
    }
    
    // Volume controls (23-25) - fifth priority
    masterSlider.setExplicitFocusOrder(23);
    leftSlider.setExplicitFocusOrder(24);
    rightSlider.setExplicitFocusOrder(25);
    
    // Advanced controls toggle (26) - sixth priority
    advancedControlsToggle.setExplicitFocusOrder(26);
    
    // Advanced controls sliders (27+) - last priority
    int focusOrder = 27;
    for (int i = 0; i < 12; ++i) {
        if (advancedPanel.sliders[i]) {
            advancedPanel.sliders[i]->setExplicitFocusOrder(focusOrder++);
        }
    }
}

void MainComponent::mouseDown(const juce::MouseEvent& e)
{
    dragStart = e.getPosition();
    startFreq = frequencyHz;
}

void MainComponent::mouseDrag(const juce::MouseEvent& e)
{
    auto delta = e.getPosition() - dragStart;
    const int dx = delta.x, dy = delta.y;

    // Horizontal: ±10 Hz per "step" of ~16 px
    if (std::abs(dx) >= 16)
    {
        int steps = dx / 16;
        setFrequency(startFreq + steps * 10.0);
    }
    // Vertical: ±1 Hz per ~12 px (note: up should increase)
    if (std::abs(dy) >= 12)
    {
        int steps = -dy / 12;
        setFrequency(frequencyHz + steps * 1.0);
    }
}

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    // Press 'T' to toggle test tone for audio debugging
    if (key.getKeyCode() == 'T' || key.getKeyCode() == 't')
    {
        toggleTestTone();
        return true;
    }
    
    // Press 'D' to show audio device info
    if (key.getKeyCode() == 'D' || key.getKeyCode() == 'd')
    {
        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            juce::String info = "Audio Device Info:\n\n";
            info += "Name: " + device->getName() + "\n";
            info += "Sample Rate: " + juce::String(device->getCurrentSampleRate()) + " Hz\n";
            info += "Buffer Size: " + juce::String(device->getCurrentBufferSizeSamples()) + " samples\n";
            info += "Output Channels: " + juce::String(device->getOutputChannelNames().size()) + "\n";
            info += "Audio Initialized: " + juce::String(isAudioInitialized ? "Yes" : "No") + "\n";
            info += "Test Tone: " + juce::String(testToneEnabled ? "Enabled" : "Disabled");
            
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Audio Device Info",
                info,
                "OK"
            );
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Audio Device Info",
                "No audio device found!",
                "OK"
            );
        }
        return true;
    }
    
    return false;
}
