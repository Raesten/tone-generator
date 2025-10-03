#include "MainComponent.h"
#include <algorithm>

#if JUCE_IOS
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#endif

MainComponent::MainComponent()
{
    // Apply minimal look and feel for general UI
    setLookAndFeel(&lookAndFeelMinimal);
    
    // Initialize STK synthesizer
    for (int i = 0; i < defaultNumVoices; ++i)
        synth.addVoice(new OscilVoice());
    
    synth.addSound(new OscilSound());
    
    setupFlexBoxLayout();
    setupWaveformControls();
    setupNoteButtons();
    setupAdvancedControlsBasic(); // Only create toggle button, defer slider creation
    
    // Set initial window size - responsive design with more space
    setSize(1000, 800); // Larger default to accommodate better spacing
    
    // Enable keyboard input for debugging
    setWantsKeyboardFocus(true);
    
    // Initialize audio immediately - JUCE AudioAppComponent requires this for proper function
    std::cout << "=== MainComponent constructor: Initializing audio for proper AudioAppComponent function ===" << std::endl;
    std::cout.flush();
    initialiseAudioSafely();
    std::cout << "=== MainComponent constructor: Audio initialization completed ===" << std::endl;
    std::cout.flush();
    
    // Initialize chorus parameters for warm, soft sound
    chorus.setRate(0.12f);        // Gentle rate for soft movement
    chorus.setDepth(0.2f);        // Subtle modulation for warmth
    chorus.setCentreDelay(0.008f); // Shorter delay for gentler effect
    chorus.setFeedback(0.05f);    // Minimal feedback for soft sound
    chorus.setMix(0.15f);         // Gentle chorus for warmth
    
    // Initialize smoothed values for warm, soft effects (50ms smoothing time)
    smoothedReverbRoomSize.reset(44100, 0.6f);   // Smaller room for warmth
    smoothedReverbDamping.reset(44100, 0.7f);    // Higher damping for softer tone
    smoothedReverbWet.reset(44100, 0.25f);       // Moderate reverb for warmth
    smoothedReverbDry.reset(44100, 0.9f);        // More dry signal for clarity
    smoothedReverbWidth.reset(44100, 0.8f);      // Narrower width for warmth
    smoothedChorusRate.reset(44100, 0.12f);      // Gentle rate for softness
    smoothedChorusDepth.reset(44100, 0.2f);      // Subtle modulation
    smoothedChorusMix.reset(44100, 0.15f);       // Gentle chorus for warmth
    
    // Initialize performance monitoring
    performanceMode = 0; // Start in normal mode
    cpuUsageCounter = 0;
    
    // Initialize state persistence (defer loading for better startup performance)
    initializePropertiesFile();
    // State loading will be done after UI construction is complete
    startTimer(50); // Load state 50ms after constructor completes
    
    // Setup accessibility focus order for basic controls only
    // Advanced controls focus order will be set when sliders are lazily loaded
    setupBasicAccessibilityFocusOrder();
}

MainComponent::~MainComponent()
{
    // Save state before shutting down
    saveState();
    
    // Clean up look and feel references
    setLookAndFeel(nullptr);
    
    // Reset individual button look and feels
    for (auto& button : noteButtons) {
        button.setLookAndFeel(nullptr);
    }
    
    shutdownAudio();
    
#if JUCE_IOS
    // Clean up audio notification observers
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
    for (size_t i = 0; i < 12; ++i)
    {
        noteButtons[i].setButtonText(noteNames[i]);
        noteButtons[i].setClickingTogglesState(true); // Enable toggle behavior - buttons stay "down" when active
        noteButtons[i].onClick = [this, i] { 
            std::cout << "LAMBDA CALLED for button " << i << std::endl;
            onNoteButtonClicked(static_cast<int>(i)); 
        };
        
        // Add press animation support
        noteButtons[i].onStateChange = [this, i] {
            bool isPressed = noteButtons[i].getToggleState();
            lookAndFeelKeyTiles.animateKeyTilePress(noteButtons[i], isPressed);
            
            // Trigger haptic feedback on press
            if (isPressed) {
                triggerHapticFeedback();
            }
        };
        
        // Apply specialized key tile look and feel for note buttons
        noteButtons[i].setLookAndFeel(&lookAndFeelKeyTiles);
        
        // VoiceOver accessibility
        noteButtons[i].setAccessible(true);
        
        playPanel.addAndMakeVisible(noteButtons[i]);
    }
    
    // Set initial button sizes
    updateButtonSizes();
}


void MainComponent::setupAdvancedControlsBasic()
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
            
            // Lazy load advanced sliders when first accessed
            if (showAdvanced && !advancedSlidersCreated) {
                std::cout << "=== Lazy loading advanced sliders for first time ===" << std::endl;
                createAdvancedSliders();
                advancedSlidersCreated = true;
                std::cout << "=== Advanced sliders lazy loading completed ===" << std::endl;
            }
            
            playPanel.setVisible(!showAdvanced);
            advancedVP.setVisible(showAdvanced);
            
            // Hide divider when showing advanced controls
            dividerComponent.setVisible(!showAdvanced);
            if (showAdvanced) dividerComponent.setBounds(0,0,0,0);
            
            if (showAdvanced) advancedVP.toFront(false); // above playPanel
            
            // Update synthesizer parameters when advanced controls are shown
            if (showAdvanced) updateAdvancedControls();
            
            // Sanity check - should never see both panels true
            juce::Logger::writeToLog(juce::String("Advanced=") + (showAdvanced ? "on" : "off") +
                 "  playPanel vis=" + (playPanel.isVisible() ? "1" : "0") +
                 "  advancedVP vis=" + (advancedVP.isVisible() ? "1" : "0"));
            
            resized(); // relayout the active panel only
        };
    advancedControlsToggle.setColour(juce::ToggleButton::tickColourId, Theme::accent);
    advancedControlsToggle.setColour(juce::ToggleButton::tickDisabledColourId, Theme::textDim);
    advancedControlsToggle.setAccessible(true);
    // Hide the toggle button to simplify UI while keeping functionality
    advancedControlsToggle.setVisible(false);
    addAndMakeVisible(advancedControlsToggle);
    
    // Initialize advanced panel viewport (but without sliders)
    addAndMakeVisible(advancedVP);
    advancedVP.setViewedComponent(&advancedPanel, false);
    advancedVP.setScrollBarsShown(false, false); // hide both scrollbars completely
    advancedVP.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::transparentBlack);
    advancedVP.getVerticalScrollBar().setColour(juce::ScrollBar::trackColourId, juce::Colours::transparentBlack);
    advancedVP.setVisible(false); // default hidden
}

void MainComponent::createAdvancedSliders()
{
    
    // Create sliders with synth parameters
    auto make = [&](int i, const juce::String& /*name*/) {
        advancedPanel.labels[static_cast<size_t>(i)] = std::make_unique<juce::Label>();
        advancedPanel.sliders[static_cast<size_t>(i)] = std::make_unique<juce::Slider>();
        auto& s = *advancedPanel.sliders[static_cast<size_t>(i)];
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        s.setColour(juce::Slider::trackColourId, Theme::knobTrack);
        s.setColour(juce::Slider::thumbColourId, Theme::knobFill);
        s.setColour(juce::Slider::textBoxTextColourId, Theme::text);
        s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        s.onValueChange = [this] { updateAdvancedControls(); };
        advancedPanel.addAndMakeVisible(s);
    };
    
    make(0, "Detune"); make(1, "Osc Mix"); make(2, "Attack"); make(3, "Decay");
    make(4, "Sustain"); make(5, "Release"); make(6, "Filter Cutoff"); make(7, "Resonance");
    make(8, "Filter Env Amt"); make(9, "Filter Env A"); make(10, "Filter Env D"); make(11, "Filter Env S");
    make(12, "Filter Env R"); make(13, "Filter LFO Depth"); make(14, "Filter LFO Rate"); make(15, "Pitch LFO Depth");
    make(16, "Pitch LFO Rate"); make(17, "Reverb Size"); make(18, "Reverb Damping"); 
    make(19, "Reverb Wet"); make(20, "Reverb Dry"); make(21, "Reverb Width");
    make(22, "Chorus Rate"); make(23, "Chorus Depth"); make(24, "Chorus Mix");
    
    // Set up slider ranges and values for lush, deep sound
    advancedPanel.sliders[0]->setRange(0.0, 2.0, 0.01); advancedPanel.sliders[0]->setValue(0.12); // Detune Amount (semitones) - More detuning for lushness
    advancedPanel.sliders[1]->setRange(1, 8, 1); advancedPanel.sliders[1]->setValue(4); // Unison Count (oscillators) - 4 oscillators for richness
        advancedPanel.sliders[2]->setRange(0.01, 0.5, 0.001); advancedPanel.sliders[2]->setValue(0.15); // Attack Time (seconds) - Slightly slower for lushness
        advancedPanel.sliders[3]->setRange(0.01, 0.3, 0.001); advancedPanel.sliders[3]->setValue(0.12); // Decay Time (seconds) - Slightly slower
        advancedPanel.sliders[4]->setRange(0.6, 0.9, 0.01); advancedPanel.sliders[4]->setValue(0.85); // Sustain Level (0-1) - Higher for lushness
        advancedPanel.sliders[5]->setRange(0.1, 1.0, 0.001); advancedPanel.sliders[5]->setValue(0.4); // Release Time (seconds) - Longer for lushness
    advancedPanel.sliders[6]->setRange(800.0, 2000.0, 1.0); advancedPanel.sliders[6]->setValue(1200.0); // Filter Cutoff (Hz) - Higher for brightness
    advancedPanel.sliders[7]->setRange(0.5, 0.8, 0.01); advancedPanel.sliders[7]->setValue(0.55); // Filter Resonance (Q) - More resonance for character
    advancedPanel.sliders[8]->setRange(0.0, 1000.0, 10.0); advancedPanel.sliders[8]->setValue(500.0); // Filter Env Amount (Hz) - More movement
    advancedPanel.sliders[9]->setRange(0.01, 1.0, 0.001); advancedPanel.sliders[9]->setValue(0.3); // Filter Env Attack (s) - Faster attack
    advancedPanel.sliders[10]->setRange(0.01, 1.0, 0.001); advancedPanel.sliders[10]->setValue(0.5); // Filter Env Decay (s) - Medium decay
    advancedPanel.sliders[11]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[11]->setValue(0.7); // Filter Env Sustain (0-1) - Higher sustain
    advancedPanel.sliders[12]->setRange(0.01, 2.0, 0.001); advancedPanel.sliders[12]->setValue(0.8); // Filter Env Release (s) - Medium release
    advancedPanel.sliders[13]->setRange(0.0, 50.0, 1.0); advancedPanel.sliders[13]->setValue(15.0); // Filter LFO Depth (Hz) - More movement
    advancedPanel.sliders[14]->setRange(0.05, 0.2, 0.001); advancedPanel.sliders[14]->setValue(0.08); // Filter LFO Rate (Hz) - Slower for lushness
    advancedPanel.sliders[15]->setRange(0.0, 2.0, 0.1); advancedPanel.sliders[15]->setValue(0.5); // Pitch LFO Depth (cents) - Subtle pitch movement
    advancedPanel.sliders[16]->setRange(4.0, 6.0, 0.1); advancedPanel.sliders[16]->setValue(4.5); // Pitch LFO Rate (Hz) - Slower for lushness
    advancedPanel.sliders[17]->setRange(0.65, 0.85, 0.01); advancedPanel.sliders[17]->setValue(0.85); // Reverb Room Size (0-1) - Large room for depth
    advancedPanel.sliders[18]->setRange(0.35, 0.5, 0.01); advancedPanel.sliders[18]->setValue(0.35); // Reverb Damping (0-1) - Less damping for lushness
    advancedPanel.sliders[19]->setRange(0.25, 0.45, 0.01); advancedPanel.sliders[19]->setValue(0.45); // Reverb Wet Level (0-1) - More reverb for lushness
    advancedPanel.sliders[20]->setRange(0.6, 1.0, 0.01); advancedPanel.sliders[20]->setValue(0.8); // Reverb Dry Level (0-1) - Less dry
    advancedPanel.sliders[21]->setRange(0.0, 1.0, 0.01); advancedPanel.sliders[21]->setValue(1.0); // Reverb Width (0-1) - Full stereo width
    advancedPanel.sliders[22]->setRange(0.15, 0.35, 0.001); advancedPanel.sliders[22]->setValue(0.15); // Chorus Rate (Hz) - Slower for lushness
    advancedPanel.sliders[23]->setRange(0.25, 0.4, 0.001); advancedPanel.sliders[23]->setValue(0.45); // Chorus Depth (0-1) - Deeper modulation
    advancedPanel.sliders[24]->setRange(0.1, 0.25, 0.001); advancedPanel.sliders[24]->setValue(0.25); // Chorus Mix (0-1) - More chorus for richness
    
    // Set up labels with proper parameter names
    juce::StringArray paramNames = {
        "Detune", "Osc Count", "Attack", "Decay", "Sustain", "Release",
        "Filter Cut", "Resonance", "Filter Env", "Filter A", "Filter D", "Filter S",
        "Filter R", "Filter LFO", "Filter Rate", "Pitch LFO", "Pitch Rate", "Reverb Size",
        "Reverb Damp", "Reverb Wet", "Reverb Dry", "Reverb Width", "Chorus Rate", "Chorus Depth", "Chorus Mix"
    };
    
    for (size_t i = 0; i < 25; ++i) { // Updated for 25 sliders
        advancedPanel.labels[i]->setText(paramNames[static_cast<int>(i)], juce::dontSendNotification);
        advancedPanel.labels[i]->setJustificationType(juce::Justification::centred);
        advancedPanel.labels[i]->setColour(juce::Label::textColourId, Theme::textDim);
        advancedPanel.labels[i]->setFont(Theme::label(0.8f)); // Smaller font for parameter names
        advancedPanel.addAndMakeVisible(advancedPanel.labels[i].get());
    }
    
    // Set up accessibility focus order for the newly created sliders
    setupAdvancedAccessibilityFocusOrder();
}




void MainComponent::setupWaveformControls()
{
    // Create waveform segment buttons but keep them hidden
    juce::StringArray shortWaves = { "Sin", "Sqr", "Tri", "Saw" };
    
    for (size_t i = 0; i < static_cast<size_t>(waves.size()); ++i)
    {
        auto* b = waveBtns.add(new juce::TextButton(shortWaves[static_cast<int>(i)]));
        b->setClickingTogglesState(true);
        b->onClick = [this, i]() { setWave(static_cast<int>(i)); };
        b->setColour(juce::TextButton::buttonColourId, Theme::btnOff);
        b->setColour(juce::TextButton::textColourOffId, Theme::text);
        b->setColour(juce::TextButton::buttonOnColourId, Theme::btnOn);
        b->setColour(juce::TextButton::textColourOnId, Theme::bg);
        
        // Set tooltip with full name for accessibility
        b->setTooltip(waves[static_cast<int>(i)]);
        
        // Add to playPanel but keep hidden
        playPanel.addAndMakeVisible(b);
        b->setVisible(false); // Hide immediately after creation
        b->setBounds(0, 0, 0, 0); // Set zero bounds
    }
    setWave(2); // Set initial waveform to Triangle for richer harmonics while maintaining warmth
}


void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    DBG("=== prepareToPlay called ===");
    DBG("Sample Rate: " << sampleRate << " Hz");
    DBG("Block Size: " << samplesPerBlockExpected);
    
    // Validate sample rate is reasonable
    if (sampleRate <= 0.0 || sampleRate < 8000.0 || sampleRate > 192000.0)
    {
        DBG("Invalid sample rate detected: " << sampleRate << " Hz, using 44100 Hz fallback");
        sampleRate = 44100.0; // Use 44.1 kHz as fallback
    }
    
    // Validate block size is reasonable
    if (samplesPerBlockExpected <= 0 || samplesPerBlockExpected > 4096)
    {
        DBG("Invalid block size: " << samplesPerBlockExpected << ", using 512 fallback");
        samplesPerBlockExpected = 512;
    }
    
    try
    {
        // Set current playback sample rate for STK synthesizer
        synth.setCurrentPlaybackSampleRate(sampleRate);
        
        // Reset keyboard state
        keyboardState.reset();
        
        // Update synthesizer parameters with current UI values
        updateAdvancedControls();
        
        // Reverb / DSP setup for lush, deep sound
        juce::dsp::ProcessSpec spec { sampleRate,
                                      (juce::uint32) juce::jlimit(32, 4096, samplesPerBlockExpected),
                                      2 };
        reverb.reset();
        reverb.prepare(spec);
        
        // Set reverb parameters for 100% wet output (auxiliary send/return)
        reverbParams.roomSize = 0.6f;     // Smaller room for softer sound
        reverbParams.damping = 0.7f;      // Higher damping for warmer, softer tone
        reverbParams.wetLevel = 1.0f;     // 100% wet output (no dry signal)
        reverbParams.dryLevel = 0.0f;     // 0% dry output (auxiliary path)
        reverbParams.width = 0.8f;        // Slightly narrower for warmth
        reverbParams.freezeMode = 0.0f;   // Normal mode
        
        reverb.setParameters(reverbParams);
        
        // Verify reverb parameters are actually set
        auto currentParams = reverb.getParameters();
        DBG("Reverb parameters set - Room: " << reverbParams.roomSize << ", Wet: " << reverbParams.wetLevel << ", Dry: " << reverbParams.dryLevel);
        DBG("Reverb parameters verified - Room: " << currentParams.roomSize << ", Wet: " << currentParams.wetLevel << ", Dry: " << currentParams.dryLevel);
        
        // Chorus setup for stereo width
        chorus.reset();
        chorus.prepare(spec);
        
        // Set chorus parameters for 100% wet output (auxiliary send/return)
        chorus.setRate(0.12f);        // Even slower rate for gentler movement
        chorus.setDepth(0.2f);        // Reduced depth for subtle modulation
        chorus.setCentreDelay(0.008f); // Shorter delay for less pronounced effect
        chorus.setFeedback(0.05f);    // Reduced feedback for softer sound
        chorus.setMix(1.0f);          // 100% wet output (auxiliary path)
        
        // Allocate effects buffer once for reuse (largest expected block size)
        const int maxBlockSize = juce::jlimit(32, 4096, samplesPerBlockExpected);
        effectsBuffer.setSize(2, maxBlockSize); // Stereo buffer for largest block size
        DBG("Effects buffer allocated: " << effectsBuffer.getNumChannels() << " channels, " << effectsBuffer.getNumSamples() << " samples");
        
        DBG("Chorus parameters set - Rate: " << 0.15f << ", Depth: " << 0.4f << ", Mix: " << 1.0f);
        
        // Configure smoothed values with 50ms smoothing time (buttery smooth)
        const float smoothingTimeMs = 50.0f; // 30-80ms range, using 50ms for buttery smooth
        smoothedReverbRoomSize.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedReverbDamping.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedReverbWet.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedReverbDry.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedReverbWidth.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedChorusRate.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedChorusDepth.reset(sampleRate, smoothingTimeMs / 1000.0f);
        smoothedChorusMix.reset(sampleRate, smoothingTimeMs / 1000.0f);
        
        // Track current device settings for effects re-preparation
        currentSampleRate = sampleRate;
        currentBlockSize = samplesPerBlockExpected;
        
        isAudioInitialized = true;
        DBG("=== Clean baseline audio prepared successfully ===");
        DBG("Sample rate: " << sampleRate << " Hz, Block size: " << samplesPerBlockExpected);
        DBG("Polyphony: " << synth.getNumVoices() << " voices");
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
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    // Clean baseline: Clear → render synth → apply master gain → FX
    bufferToFill.clearActiveBufferRegion();
    
    // Performance optimization: Denormal protection for older devices
    juce::ScopedNoDenormals noDenormals;
    
    try
    {
        // 1. Render STK synthesizer audio with MIDI from keyboard state
        juce::MidiBuffer midiBuffer;
        keyboardState.processNextMidiBuffer(midiBuffer, bufferToFill.startSample, bufferToFill.numSamples, true);
        
        // Debug: Log MIDI messages to see what's being sent
        static int midiDebugCounter = 0;
        if (midiDebugCounter < 10) { // Only log first 10 callbacks with MIDI
            if (!midiBuffer.isEmpty()) {
                DBG("=== MIDI BUFFER DEBUG ===");
                for (const auto metadata : midiBuffer) {
                    auto message = metadata.getMessage();
                    if (message.isNoteOn()) {
                        DBG("MIDI Note ON: " << message.getNoteNumber() << " (" << juce::MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 4) << "), velocity: " << message.getVelocity());
                    } else if (message.isNoteOff()) {
                        DBG("MIDI Note OFF: " << message.getNoteNumber() << " (" << juce::MidiMessage::getMidiNoteName(message.getNoteNumber(), true, true, 4) << "), velocity: " << message.getVelocity());
                    }
                }
                midiDebugCounter++;
            }
        }
        
        // Debug: Always show main audio callback activity
        static int callCount = 0;
        callCount++;
        
        DBG("MAIN AUDIO CALLBACK #" << callCount << ": numSamples=" << bufferToFill.numSamples << ", channels=" << bufferToFill.buffer->getNumChannels());
        
        // Debug: Check buffer before synth rendering
        float preRms = 0.0f;
        for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch) {
            for (int i = 0; i < bufferToFill.numSamples; ++i) {
                float sample = bufferToFill.buffer->getSample(ch, bufferToFill.startSample + i);
                preRms += sample * sample;
            }
        }
        preRms = std::sqrt(preRms / (bufferToFill.numSamples * bufferToFill.buffer->getNumChannels()));
        DBG("MAIN AUDIO: Before synth render - RMS: " << preRms);
        
        // Debug: Count active voices before rendering
        int activeVoices = 0;
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            if (auto* voice = dynamic_cast<OscilVoice*>(synth.getVoice(i))) {
                if (voice->isVoiceActive()) {
                    activeVoices++;
                }
            }
        }
        
        synth.renderNextBlock(*bufferToFill.buffer, midiBuffer, bufferToFill.startSample, bufferToFill.numSamples);
        
        // Debug: Check buffer after synth rendering
        float postRms = 0.0f;
        for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch) {
            for (int i = 0; i < bufferToFill.numSamples; ++i) {
                float sample = bufferToFill.buffer->getSample(ch, bufferToFill.startSample + i);
                postRms += sample * sample;
            }
        }
        postRms = std::sqrt(postRms / (bufferToFill.numSamples * bufferToFill.buffer->getNumChannels()));
        DBG("MAIN AUDIO: After synth render - RMS: " << postRms << " (Active voices: " << activeVoices << ")");
        
        // 1.5. Performance monitoring and optimization
        adjustPerformanceSettings();
        
        // Apply gentle gain reduction for softer sound
        bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, 0.8f);
        
        // 2.5. Update smoothed parameters in real-time for buttery smooth transitions
        // NOTE: Effects parameters are now set once in prepareToPlay for optimal performance
        // Only update if UI parameters have changed (handled elsewhere)
        
        // 3. Apply FX as Auxiliary Send/Return (Fixed Implementation)
        // Use pre-allocated effects buffer to avoid real-time allocations
        if (effectsBuffer.getNumSamples() >= bufferToFill.numSamples)
        {
            // Clear the effects buffer for this block
            effectsBuffer.clear();
            
            // Copy dry signal to effects buffer (auxiliary send)
            // Ensure we handle both mono and stereo consistently
            const int numInputChannels = bufferToFill.buffer->getNumChannels();
            const int numEffectsChannels = effectsBuffer.getNumChannels();
            
            for (int ch = 0; ch < juce::jmin(numInputChannels, numEffectsChannels); ++ch) {
                effectsBuffer.copyFrom(ch, 0, *bufferToFill.buffer, ch, bufferToFill.startSample, bufferToFill.numSamples);
            }
            
            // If input is mono but effects buffer is stereo, duplicate the mono signal
            if (numInputChannels == 1 && numEffectsChannels == 2) {
                effectsBuffer.copyFrom(1, 0, effectsBuffer, 0, 0, bufferToFill.numSamples);
            }
            
            // Create audio block for effects processing with correct channel count
            const int channelsToProcess = juce::jmin(numInputChannels, numEffectsChannels);
            juce::dsp::AudioBlock<float> effectsBlock(effectsBuffer.getArrayOfWritePointers(), channelsToProcess, bufferToFill.numSamples);
            juce::dsp::ProcessContextReplacing<float> effectsContext(effectsBlock);
            
            // Process effects in block (reverb first, then chorus)
            reverb.process(effectsContext);
            chorus.process(effectsContext);
            
            // Mix effects back with dry signal (auxiliary return)
            // Effects are now 100% wet, so we add them to the dry signal
            const float effectsReturn = 0.15f; // 15% return from effects for softer sound
            
            for (int ch = 0; ch < juce::jmin(numInputChannels, channelsToProcess); ++ch) {
                bufferToFill.buffer->addFrom(ch, bufferToFill.startSample, effectsBuffer, ch, 0, bufferToFill.numSamples, effectsReturn);
            }
        }
        else
        {
            // Effects buffer too small - skip effects processing for this block
            DBG("WARNING: Effects buffer too small (" << effectsBuffer.getNumSamples() << " < " << bufferToFill.numSamples << "), skipping effects");
        }
        
        // Debug: Check FINAL buffer after all processing
        float finalRms = 0.0f;
        for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch) {
            for (int i = 0; i < bufferToFill.numSamples; ++i) {
                float sample = bufferToFill.buffer->getSample(ch, bufferToFill.startSample + i);
                finalRms += sample * sample;
            }
        }
        finalRms = std::sqrt(finalRms / (bufferToFill.numSamples * bufferToFill.buffer->getNumChannels()));
        DBG("MAIN AUDIO: FINAL OUTPUT - RMS: " << finalRms);
        
        // 5. RMS logging to verify non-zero audio (once per second)
        static int rmsCounter = 0;
        static const int rmsLogInterval = 44100 / bufferToFill.numSamples; // Log once per second
        if (++rmsCounter >= rmsLogInterval)
        {
            rmsCounter = 0;
            
            // Calculate RMS for verification
            float rms = 0.0f;
            for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            {
                float channelRms = 0.0f;
                for (int i = 0; i < bufferToFill.numSamples; ++i)
                {
                    float sample = bufferToFill.buffer->getSample(channel, bufferToFill.startSample + i);
                    channelRms += sample * sample;
                }
                channelRms = std::sqrt(channelRms / bufferToFill.numSamples);
                rms = juce::jmax(rms, channelRms);
            }
            
            DBG("Audio RMS: " << rms);
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
    synth.allNotesOff(1, true); // Channel 1, allow tail off for graceful release
    keyboardState.reset();
}

void MainComponent::paint(juce::Graphics& g)
{
    // Draw gradient background with optional radial vignette
    BackgroundGradient::drawCanvasBackground(g, getLocalBounds());
}

void MainComponent::resized()
{
    auto root = getLocalBounds();
    
    // Apply safe-area padding for notch/rounded corners
    auto sa = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->safeAreaInsets;
    root = root.withTrimmedTop(sa.getTop()).withTrimmedBottom(sa.getBottom())
               .withTrimmedLeft(sa.getLeft()).withTrimmedRight(sa.getRight());

    const int gap = 12;

    // Header spacing
    root.removeFromTop(8);

    // Advanced controls toggle (hidden to simplify UI)
    // auto toggleArea = root.removeFromTop(44);
    advancedControlsToggle.setBounds(0, 0, 0, 0); // Set zero bounds since it's hidden
    
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
        const int innerGap = 12;
        const int rows = 4; /*, cols = 3;*/
        const int rowH = 100; // slider column height; adjust to taste
        const int innerH = rows * rowH + (rows - 1) * innerGap + 24;

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

        layoutNotes(panelArea); // grid code goes here
        
        // Debug logging for verification
        juce::Logger::writeToLog("Advanced Controls OFF: playPanel vis=" + juce::String((int)playPanel.isVisible()) 
            + ", advancedVP vis=" + juce::String((int)advancedVP.isVisible()));
    }
    
    // Update responsive layout parameters
    updateResponsiveLayout();
}

void MainComponent::layoutNotes(juce::Rectangle<int> bounds)
{
    // Ensure minimum height for the layout
    if (bounds.getHeight() < 400) {
        DBG("Warning: Layout bounds too small: " << bounds.toString());
        return; // Don't layout if too small
    }
    
    // Middle card area - ensure we have enough space
    auto mid = bounds.withTrimmedBottom(Theme::playHeight + 64);
    
    // Hide waveform buttons - no longer part of the GUI
    for (auto* btn : waveBtns) {
        btn->setBounds(0, 0, 0, 0);
        btn->setVisible(false);
    }

    mid.removeFromTop(12); // Small gap

    // ---- NOTE GRID ---- (Centered vertically)
    const int minTap = Theme::keyMinHitSize; // Use theme minimum hit size
    const int gap = Theme::colGap; // Use theme column gap
    const int pageMargin = Theme::pageMargin; // Use theme page margins

    // Calculate available width with proper margins
    int availableWidth = bounds.getWidth() - (2 * pageMargin);
    
    // Force 3x4 grid layout (3 columns, 4 rows)
    int cols = 3;
    int rows = (12 + cols - 1) / cols;
    int tile = juce::jmax(minTap, (availableWidth - (cols - 1) * gap) / cols);
    
    // Calculate grid dimensions
    int gridWidth = cols * tile + (cols - 1) * gap;
    int gridHeight = rows * tile + (rows - 1) * gap;
    
    // Center the grid in the available bounds with proper margins
    auto noteArea = bounds.withSizeKeepingCentre(gridWidth, gridHeight);
    noteArea = noteArea.withTrimmedLeft(pageMargin).withTrimmedRight(pageMargin);

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

    noteGrid.performLayout(noteArea);
    
    // Debug: Check if buttons have proper bounds and are visible
    DBG("=== BUTTON LAYOUT DEBUG ===");
    for (int i = 0; i < 12; ++i) {
        auto bounds = noteButtons[i].getBounds();
        DBG("Button " << i << ": bounds=" << bounds.toString() << ", visible=" << (noteButtons[i].isVisible() ? "true" : "false"));
    }
    DBG("PlayPanel bounds: " << playPanel.getBounds().toString() << ", visible=" << (playPanel.isVisible() ? "true" : "false"));
    
    // Divider component
    auto dividerArea = bounds.removeFromTop(1);
    dividerComponent.setBounds(dividerArea);
}

void MainComponent::layoutCompactNoteGrid(juce::Rectangle<int> bounds)
{
    // Simple compact layout - just the note grid
    const int minTap = 44;
    const int gap = 8;
    
    // 3x4 grid layout
    int cols = 3;
    int rows = (12 + cols - 1) / cols;
    int tile = juce::jmax(minTap, (bounds.getWidth() - (cols - 1) * gap) / cols);
    
    // Center the grid
    int gridWidth = cols * tile + (cols - 1) * gap;
    int gridHeight = rows * tile + (rows - 1) * gap;
    auto gridArea = bounds.withSizeKeepingCentre(gridWidth, gridHeight);
    
    // Layout note buttons in grid
    int buttonIndex = 0;
    for (int row = 0; row < rows && buttonIndex < 12; ++row) {
        for (int col = 0; col < cols && buttonIndex < 12; ++col) {
            int x = gridArea.getX() + col * (tile + gap);
            int y = gridArea.getY() + row * (tile + gap);
            noteButtons[buttonIndex].setBounds(x, y, tile, tile);
            ++buttonIndex;
        }
    }
    
    // Hide waveform buttons and volume controls in compact mode
    for (auto* btn : waveBtns) {
        btn->setBounds(0, 0, 0, 0);
        btn->setVisible(false);
    }
}

void MainComponent::onNoteButtonClicked(int noteIndex)
{
    // Force debug output to console
    std::cout << "=== NOTE BUTTON CLICKED: " << noteIndex << " ===" << std::endl;
    std::cout << "isAudioInitialized: " << (isAudioInitialized ? "true" : "false") << std::endl;
    
    DBG("=== NOTE BUTTON CLICKED: " << noteIndex << " ===");
    DBG("isAudioInitialized: " << (isAudioInitialized ? "true" : "false"));
    
    // Add haptic feedback on iOS
    triggerHapticFeedback();
    
    // Force immediate repaint for instant visual feedback
    noteButtons[noteIndex].repaint();
    
    // Audio is now initialized in constructor, just check if it's ready
    if (!isAudioInitialized)
    {
        DBG("Audio not initialized - returning early");
        // Audio initialization failed - user was already notified
        return;
    }
    
    // Hook the UI to MIDI note numbers and velocities that aren't zero
    const int midiNote = 60 + noteIndex;   // C4 + offset
    const bool on = noteButtons[noteIndex].getToggleState();

    DBG("Button " << noteIndex << " toggle state: " << (on ? "ON" : "OFF"));
    
    // Debug: Show current state of all buttons
    DBG("=== ALL BUTTON STATES ===");
    for (int i = 0; i < 12; ++i) {
        bool buttonState = noteButtons[i].getToggleState();
        DBG("Button " << i << ": " << (buttonState ? "ON" : "OFF"));
    }
    DBG("=== END BUTTON STATES ===");

    if (on)  
    {
        DBG("=== Triggering note ON: " << midiNote << " (velocity: 0.9) ===");
        
        // Debug: Check voice allocation before triggering note
        int availableVoices = 0;
        int busyVoices = 0;
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            if (auto* voice = dynamic_cast<OscilVoice*>(synth.getVoice(i))) {
                if (voice->isVoiceActive()) {
                    busyVoices++;
                } else {
                    availableVoices++;
                }
            }
        }
        DBG("Before note ON - Busy voices: " << busyVoices << ", Available voices: " << availableVoices);
        
        keyboardState.noteOn(1, midiNote, 0.9f);
    }
    else     
    {
        DBG("=== Triggering note OFF: " << midiNote << " (velocity: 0.9) ===");
        keyboardState.noteOff(1, midiNote, 0.9f);
    }
    
    // Debug: Show keyboard state
    DBG("KeyboardState - isNoteOn for note " << midiNote << ": " << (keyboardState.isNoteOn(1, midiNote) ? "true" : "false"));
    
    // If we're not currently playing, start playing automatically
    if (on && !isPlaying)
    {
        isPlaying = true;
    }
    else if (!on)
    {
        // Check if any notes are still selected
        bool anyNotesSelected = false;
        for (size_t i = 0; i < 12; ++i)
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
        }
    }
    
    // Update UI feedback
    updateNoteButtonVisualFeedback();
    
    // Save state
    saveState();
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


void MainComponent::setWave(int idx)
{
    // Add haptic feedback on iOS
    triggerHapticFeedback();
    
    waveIndex = juce::jlimit(0, waves.size() - 1, idx);
    for (size_t i = 0; i < waveBtns.size(); ++i)
        waveBtns[i]->setToggleState(i == waveIndex, juce::dontSendNotification);
    
    // Update STK synthesizer oscillator type
    ParameterHolder::inst().currentOsc = waveIndex;
    
    // Save state
    saveState();
}

void MainComponent::updateAdvancedControls()
{
    DBG("=== updateAdvancedControls called ===");
    
    // Correct mapping for our 25 warmth parameters:
    // 0: Detune, 1: Osc Count, 2: Attack, 3: Decay, 4: Sustain, 5: Release,
    // 6: Filter Cut, 7: Resonance, 8: Filter Env, 9: Filter A, 10: Filter D, 11: Filter S,
    // 12: Filter R, 13: Filter LFO, 14: Filter Rate, 15: Pitch LFO, 16: Pitch Rate
    
    // Volume ADSR parameters (slots 2-5)
    if (advancedPanel.sliders[2]) ParameterHolder::inst().parameters[VOLUME_A_PARAM].store(static_cast<float>(advancedPanel.sliders[2]->getValue()));
    if (advancedPanel.sliders[3]) ParameterHolder::inst().parameters[VOLUME_D_PARAM].store(static_cast<float>(advancedPanel.sliders[3]->getValue()));
    if (advancedPanel.sliders[4]) ParameterHolder::inst().parameters[VOLUME_S_PARAM].store(static_cast<float>(advancedPanel.sliders[4]->getValue()));
    if (advancedPanel.sliders[5]) ParameterHolder::inst().parameters[VOLUME_R_PARAM].store(static_cast<float>(advancedPanel.sliders[5]->getValue()));
    
    // Filter ADSR parameters (slots 9-12)
    if (advancedPanel.sliders[9]) ParameterHolder::inst().parameters[FILTER_A_PARAM].store(static_cast<float>(advancedPanel.sliders[9]->getValue()));
    if (advancedPanel.sliders[10]) ParameterHolder::inst().parameters[FILTER_D_PARAM].store(static_cast<float>(advancedPanel.sliders[10]->getValue()));
    if (advancedPanel.sliders[11]) ParameterHolder::inst().parameters[FILTER_S_PARAM].store(static_cast<float>(advancedPanel.sliders[11]->getValue()));
    if (advancedPanel.sliders[12]) ParameterHolder::inst().parameters[FILTER_R_PARAM].store(static_cast<float>(advancedPanel.sliders[12]->getValue()));
    
    // Filter parameters (slots 6-7 for cutoff/resonance)
    if (advancedPanel.sliders[6]) ParameterHolder::inst().parameters[FILTER_START_PARAM].store(static_cast<float>(advancedPanel.sliders[6]->getValue()));
    if (advancedPanel.sliders[7]) ParameterHolder::inst().parameters[FILTER_END_PARAM].store(static_cast<float>(advancedPanel.sliders[6]->getValue())); // Use same for both
    
    // LFO parameters (slots 13-14)
    if (advancedPanel.sliders[14]) ParameterHolder::inst().parameters[LFO_RATE_PARAM].store(static_cast<float>(advancedPanel.sliders[14]->getValue()));
    if (advancedPanel.sliders[13]) ParameterHolder::inst().parameters[LFO_AMP_PARAM].store(static_cast<float>(advancedPanel.sliders[13]->getValue()));
    
    // Note: Detune (slot 0) and Osc Count (slot 1) need to be handled differently
    // as the STK system doesn't have direct equivalents for these parameters
    
    // Reverb controls - use smoothed values for buttery smooth transitions
    bool reverbChanged = false;
    if (advancedPanel.sliders[17]) {
        smoothedReverbRoomSize.setTargetValue(static_cast<float>(advancedPanel.sliders[17]->getValue()));
        reverbChanged = true;
    }
    if (advancedPanel.sliders[18]) {
        smoothedReverbDamping.setTargetValue(static_cast<float>(advancedPanel.sliders[18]->getValue()));
        reverbChanged = true;
    }
    if (advancedPanel.sliders[19]) {
        smoothedReverbWet.setTargetValue(static_cast<float>(advancedPanel.sliders[19]->getValue()));
        reverbChanged = true;
    }
    if (advancedPanel.sliders[20]) {
        smoothedReverbDry.setTargetValue(static_cast<float>(advancedPanel.sliders[20]->getValue()));
        reverbChanged = true;
    }
    if (advancedPanel.sliders[21]) {
        smoothedReverbWidth.setTargetValue(static_cast<float>(advancedPanel.sliders[21]->getValue()));
        reverbChanged = true;
    }
    
    // Chorus controls - use smoothed values for buttery smooth transitions
    bool chorusChanged = false;
    if (advancedPanel.sliders[22]) {
        smoothedChorusRate.setTargetValue(static_cast<float>(advancedPanel.sliders[22]->getValue()));
        chorusChanged = true;
    }
    if (advancedPanel.sliders[23]) {
        smoothedChorusDepth.setTargetValue(static_cast<float>(advancedPanel.sliders[23]->getValue()));
        chorusChanged = true;
    }
    if (advancedPanel.sliders[24]) {
        smoothedChorusMix.setTargetValue(static_cast<float>(advancedPanel.sliders[24]->getValue()));
        chorusChanged = true;
    }
    
    // Update effects parameters only when reverb or chorus controls change
    if (reverbChanged || chorusChanged) {
        updateEffectsParameters();
    }
    
    // Save state when advanced controls change
    saveState();
}

void MainComponent::updateEffectsParameters()
{
    // Only update effects parameters if device settings have changed
    // This method should be called when UI controls change, not every audio block
    
    // Update reverb parameters with current smoothed values
    reverbParams.roomSize = smoothedReverbRoomSize.getCurrentValue();
    reverbParams.damping = smoothedReverbDamping.getCurrentValue();
    reverbParams.wetLevel = 1.0f; // Always 100% wet for auxiliary send/return
    reverbParams.dryLevel = 0.0f; // Always 0% dry for auxiliary send/return
    reverbParams.width = smoothedReverbWidth.getCurrentValue();
    reverb.setParameters(reverbParams);
    
    // Update chorus parameters with current smoothed values
    chorus.setRate(smoothedChorusRate.getCurrentValue());
    chorus.setDepth(smoothedChorusDepth.getCurrentValue());
    chorus.setMix(1.0f); // Always 100% wet for auxiliary send/return
    
    DBG("Effects parameters updated - Reverb: " << reverbParams.roomSize << ", Chorus: " << smoothedChorusRate.getCurrentValue());
}

void MainComponent::updateNoteButtonVisualFeedback()
{
    // Update visual feedback for note buttons based on playback state
    for (size_t i = 0; i < 12; ++i)
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
    
    
    
    // Control button sizing variables removed (no longer needed)
    
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
    
    
    // Update title font size for responsive design
    // This will be applied in the paint() method
    
    // DON'T call resized() here - it creates infinite recursion!
    // The layout will be updated when the component is actually resized
}

void MainComponent::timerCallback()
{
    // Stop the timer - we only want to load state once
    stopTimer();
    
    std::cout << "=== Timer callback: Loading saved state after UI construction ===" << std::endl;
    std::cout.flush();
    
    // Load state now that UI is ready
    loadState();
    
    // Update synthesizer parameters with loaded UI values
    updateAdvancedControls();
    
    std::cout << "=== Timer callback: State loading completed ===" << std::endl;
    std::cout.flush();
}

void MainComponent::initialiseAudioSafely()
{
    std::cout << "initialiseAudioSafely() called" << std::endl;
    std::cout.flush();
    
#if JUCE_IOS
    // Proper iOS audio setup - configure AVAudioSession first
    @autoreleasepool {
        NSError* err = nil;
        AVAudioSession* s = [AVAudioSession sharedInstance];
        
        // Set category to Playback so hardware mute switch doesn't silence us
        [s setCategory:AVAudioSessionCategoryPlayback withOptions:0 error:&err];
        if (err != nil) {
            DBG("AVAudioSession category error: " << [[err localizedDescription] UTF8String]);
        }
        
        // Set preferred sample rate (iOS will use the actual device rate)
        [s setPreferredSampleRate:44100 error:&err];
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
        DBG("iOS Audio Route: " << [[[s currentRoute] description] UTF8String]);
        DBG("iOS Output Volume: " << [s outputVolume]);
        }
        
        // Set up interruption notifications for proper audio handling
        [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification* notification) {
            onAudioInterruption((__bridge void*)notification);
        }];
    }
    
    // CRITICAL: Call setAudioChannels immediately after AVAudioSession setup
    // This is the key fix - JUCE AudioAppComponent expects this to be called early
    DBG("iOS: Calling setAudioChannels(0, 2) immediately after AVAudioSession setup");
    setAudioChannels(0, 2);   // This opens the device and triggers prepareToPlay
    
#else
    // Non-iOS platforms: set audio channels immediately
    std::cout << "Non-iOS platform: Calling setAudioChannels(0, 2) immediately" << std::endl;
    std::cout.flush();
    setAudioChannels(0, 2);
#endif
}


void MainComponent::onAudioRouteChanged()
{
    // Route change handling is now simplified - JUCE handles most of this automatically
    // when setAudioChannels is called properly in the constructor
    DBG("Audio route changed - JUCE will handle this automatically");
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
            synth.allNotesOff(1, true); // Graceful release
            deviceManager.closeAudioDevice();
            isAudioInitialized = false;
            
            // Update UI to reflect stopped state
            isPlaying = false;
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
        
        // Use light haptic feedback for subtle tactile response
        [lightGenerator prepare];
        [lightGenerator impactOccurred];
        
        DBG("Light haptic feedback triggered on device");
    }
    else
    {
        DBG("Haptic feedback not available in simulator");
    }
#endif
}

void MainComponent::toggleTestTone()
{
    // Test tone functionality removed for clean baseline
    DBG("Test tone functionality removed - using clean synthesizer baseline");
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
    
    // Volume settings removed from persistence
    
    // Save selected notes
    juce::StringArray selectedNotes;
    for (size_t i = 0; i < 12; ++i)
    {
        if (noteButtons[i].getToggleState())
        {
            selectedNotes.add(juce::String(i));
        }
    }
    propertiesFile->setValue("selectedNotes", selectedNotes.joinIntoString(","));
    
    
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
    if (advancedPanel.sliders[8]) propertiesFile->setValue("filterEnvelopeAmount", advancedPanel.sliders[8]->getValue());
    if (advancedPanel.sliders[9]) propertiesFile->setValue("filterEnvelopeAttack", advancedPanel.sliders[9]->getValue());
    if (advancedPanel.sliders[10]) propertiesFile->setValue("filterEnvelopeDecay", advancedPanel.sliders[10]->getValue());
    if (advancedPanel.sliders[11]) propertiesFile->setValue("filterEnvelopeSustain", advancedPanel.sliders[11]->getValue());
    if (advancedPanel.sliders[12]) propertiesFile->setValue("filterEnvelopeRelease", advancedPanel.sliders[12]->getValue());
    if (advancedPanel.sliders[13]) propertiesFile->setValue("filterLFODepth", advancedPanel.sliders[13]->getValue());
    if (advancedPanel.sliders[14]) propertiesFile->setValue("filterLFORate", advancedPanel.sliders[14]->getValue());
    if (advancedPanel.sliders[15]) propertiesFile->setValue("pitchLFODepth", advancedPanel.sliders[15]->getValue());
    if (advancedPanel.sliders[16]) propertiesFile->setValue("pitchLFORate", advancedPanel.sliders[16]->getValue());
    if (advancedPanel.sliders[17]) propertiesFile->setValue("reverbRoomSize", advancedPanel.sliders[17]->getValue());
    if (advancedPanel.sliders[18]) propertiesFile->setValue("reverbDamping", advancedPanel.sliders[18]->getValue());
    if (advancedPanel.sliders[19]) propertiesFile->setValue("reverbWet", advancedPanel.sliders[19]->getValue());
    if (advancedPanel.sliders[20]) propertiesFile->setValue("reverbDry", advancedPanel.sliders[20]->getValue());
    if (advancedPanel.sliders[21]) propertiesFile->setValue("reverbWidth", advancedPanel.sliders[21]->getValue());
    if (advancedPanel.sliders[22]) propertiesFile->setValue("chorusRate", advancedPanel.sliders[22]->getValue());
    if (advancedPanel.sliders[23]) propertiesFile->setValue("chorusDepth", advancedPanel.sliders[23]->getValue());
    if (advancedPanel.sliders[24]) propertiesFile->setValue("chorusMix", advancedPanel.sliders[24]->getValue());
    
    // Save the file
    propertiesFile->saveIfNeeded();
    DBG("State saved successfully");
}

void MainComponent::loadState()
{
    if (propertiesFile == nullptr) return;
    
    // Load waveform selection
    waveIndex = propertiesFile->getIntValue("waveform", 2); // Default to Triangle wave for warmth with richer harmonics
    setWave(waveIndex);
    
    // Volume settings removed from persistence
    
    // Load selected notes
    juce::String selectedNotesStr = propertiesFile->getValue("selectedNotes", "");
    if (selectedNotesStr.isNotEmpty())
    {
        juce::StringArray selectedNotes;
        selectedNotes.addTokens(selectedNotesStr, ",", "");
        
        // Clear all notes first
        for (size_t i = 0; i < 12; ++i)
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
    
    
    // Load advanced controls visibility
    showAdvanced = propertiesFile->getBoolValue("showAdvancedControls", false);
    advancedControlsToggle.setToggleState(showAdvanced, juce::dontSendNotification);
    
    // Load advanced control values with lush, deep sound defaults
    if (advancedPanel.sliders[0]) advancedPanel.sliders[0]->setValue(propertiesFile->getDoubleValue("detune", 0.12)); // More detuning for lushness
        if (advancedPanel.sliders[1]) advancedPanel.sliders[1]->setValue(propertiesFile->getDoubleValue("oscillatorCount", 4.0)); // 4 oscillators for richness
        if (advancedPanel.sliders[2]) advancedPanel.sliders[2]->setValue(propertiesFile->getDoubleValue("attack", 0.15)); // Slightly slower for lushness
        if (advancedPanel.sliders[3]) advancedPanel.sliders[3]->setValue(propertiesFile->getDoubleValue("decay", 0.12)); // Slightly slower
        if (advancedPanel.sliders[4]) advancedPanel.sliders[4]->setValue(propertiesFile->getDoubleValue("sustain", 0.85)); // Higher for lushness
        if (advancedPanel.sliders[5]) advancedPanel.sliders[5]->setValue(propertiesFile->getDoubleValue("release", 0.4)); // Longer for lushness
    if (advancedPanel.sliders[6]) advancedPanel.sliders[6]->setValue(propertiesFile->getDoubleValue("filterCutoff", 1200.0)); // Higher for brightness
    if (advancedPanel.sliders[7]) advancedPanel.sliders[7]->setValue(propertiesFile->getDoubleValue("filterResonance", 0.55)); // More resonance for character
    if (advancedPanel.sliders[8]) advancedPanel.sliders[8]->setValue(propertiesFile->getDoubleValue("filterEnvelopeAmount", 500.0)); // More movement
    if (advancedPanel.sliders[9]) advancedPanel.sliders[9]->setValue(propertiesFile->getDoubleValue("filterEnvelopeAttack", 0.3)); // Faster attack
    if (advancedPanel.sliders[10]) advancedPanel.sliders[10]->setValue(propertiesFile->getDoubleValue("filterEnvelopeDecay", 0.5)); // Medium decay
    if (advancedPanel.sliders[11]) advancedPanel.sliders[11]->setValue(propertiesFile->getDoubleValue("filterEnvelopeSustain", 0.7)); // Higher sustain
    if (advancedPanel.sliders[12]) advancedPanel.sliders[12]->setValue(propertiesFile->getDoubleValue("filterEnvelopeRelease", 0.8)); // Medium release
    if (advancedPanel.sliders[13]) advancedPanel.sliders[13]->setValue(propertiesFile->getDoubleValue("filterLFODepth", 15.0)); // More movement
    if (advancedPanel.sliders[14]) advancedPanel.sliders[14]->setValue(propertiesFile->getDoubleValue("filterLFORate", 0.08)); // Slower for lushness
    if (advancedPanel.sliders[15]) advancedPanel.sliders[15]->setValue(propertiesFile->getDoubleValue("pitchLFODepth", 0.5)); // Subtle pitch movement
    if (advancedPanel.sliders[16]) advancedPanel.sliders[16]->setValue(propertiesFile->getDoubleValue("pitchLFORate", 4.5)); // Slower for lushness
    if (advancedPanel.sliders[17]) advancedPanel.sliders[17]->setValue(propertiesFile->getDoubleValue("reverbRoomSize", 0.85)); // Large room for depth
    if (advancedPanel.sliders[18]) advancedPanel.sliders[18]->setValue(propertiesFile->getDoubleValue("reverbDamping", 0.35)); // Less damping for lushness
    if (advancedPanel.sliders[19]) advancedPanel.sliders[19]->setValue(propertiesFile->getDoubleValue("reverbWet", 0.45)); // More reverb for lushness
    if (advancedPanel.sliders[20]) advancedPanel.sliders[20]->setValue(propertiesFile->getDoubleValue("reverbDry", 0.8)); // Less dry
    if (advancedPanel.sliders[21]) advancedPanel.sliders[21]->setValue(propertiesFile->getDoubleValue("reverbWidth", 1.0)); // Full stereo width
    if (advancedPanel.sliders[22]) advancedPanel.sliders[22]->setValue(propertiesFile->getDoubleValue("chorusRate", 0.15)); // Slower for lushness
    if (advancedPanel.sliders[23]) advancedPanel.sliders[23]->setValue(propertiesFile->getDoubleValue("chorusDepth", 0.45)); // Deeper modulation
    if (advancedPanel.sliders[24]) advancedPanel.sliders[24]->setValue(propertiesFile->getDoubleValue("chorusMix", 0.25)); // More chorus for richness
    
    // Update synthesizer with loaded values
    updateAdvancedControls();
    
    // Update UI feedback
    updateNoteButtonVisualFeedback();
    
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

void MainComponent::setupBasicAccessibilityFocusOrder()
{
    // Set up focus order for basic controls only (fast startup)
    // Advanced sliders will get their focus order when lazily loaded
    
    // Note buttons (0-11) - first priority
    for (size_t i = 0; i < 12; ++i)
    {
        noteButtons[i].setExplicitFocusOrder(i + 1);
    }
    
    // Waveform controls (15-18) - second priority
    for (size_t i = 0; i < waveBtns.size(); ++i)
    {
        waveBtns[i]->setExplicitFocusOrder(15 + i);
    }
    
    // Advanced controls toggle (19) - hidden, no focus order needed
    // advancedControlsToggle.setExplicitFocusOrder(19);
}

void MainComponent::setupAdvancedAccessibilityFocusOrder()
{
    // Set up focus order for advanced controls sliders (called during lazy loading)
    int focusOrder = 20;
    for (size_t i = 0; i < 25; ++i) { // Updated for 25 sliders
        if (advancedPanel.sliders[i]) {
            advancedPanel.sliders[i]->setExplicitFocusOrder(focusOrder++);
        }
    }
}


bool MainComponent::keyPressed(const juce::KeyPress& key)
{
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
            info += "Polyphony: " + juce::String(synth.getNumVoices()) + " voices";
            
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

// Performance optimization methods
void MainComponent::adjustPerformanceSettings()
{
    // Monitor CPU usage periodically
    if (++cpuUsageCounter >= CPU_MONITOR_INTERVAL)
    {
        cpuUsageCounter = 0;
        
        // Simple CPU monitoring: check if we're using too many voices
        int activeVoices = 0;
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = synth.getVoice(i))
            {
                if (voice->isVoiceActive())
                    activeVoices++;
            }
        }
        
        // Adjust performance mode based on active voices
        if (activeVoices > 8 && performanceMode < 2)
        {
            setPerformanceMode(performanceMode + 1);
            DBG("Performance mode increased to: " << performanceMode);
        }
        else if (activeVoices <= 4 && performanceMode > 0)
        {
            setPerformanceMode(performanceMode - 1);
            DBG("Performance mode decreased to: " << performanceMode);
        }
    }
}

void MainComponent::setPerformanceMode(int mode)
{
    performanceMode = juce::jlimit(0, 2, mode);
    
    // Apply performance optimizations based on mode
    switch (performanceMode)
    {
        case 0: // Normal mode - full quality
            // No optimizations needed
            break;
            
        case 1: // Reduced mode - reduce unison count and chorus depth
            // Reduce unison count to 4 oscillators
            // Note: STK synthesizer doesn't have oscillator count setting
            // Reduce chorus depth
            smoothedChorusDepth.setTargetValue(0.2f);
            break;
            
        case 2: // Minimal mode - further reductions
            // Reduce unison count to 2 oscillators
            // Note: STK synthesizer doesn't have oscillator count setting
            // Further reduce chorus depth
            smoothedChorusDepth.setTargetValue(0.1f);
            // Reduce reverb wet level
            smoothedReverbWet.setTargetValue(0.2f);
            break;
    }
}

