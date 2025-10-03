#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Synth.h"
#include "Theme.h"
#include "LookAndFeelMinimal.h"
#include "LookAndFeelKeyTiles.h"

// Using the new LookAndFeelMinimal for cleaner, more minimal design
// Using LookAndFeelKeyTiles for specialized note key tile rendering

class MainComponent : public juce::AudioAppComponent, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void layoutNotes(juce::Rectangle<int> bounds);
    void layoutCompactNoteGrid(juce::Rectangle<int> bounds);
    
    
    // Keyboard support for debugging
    bool keyPressed(const juce::KeyPress& key) override;
    
    // Timer callback for deferred audio initialization
    void timerCallback() override;

private:
    // STK-based synthesizer
    juce::Synthesiser synth;
    juce::MidiKeyboardState keyboardState;
    
    // Custom look and feel
    LookAndFeelMinimal lookAndFeelMinimal;
    LookAndFeelKeyTiles lookAndFeelKeyTiles;
    
    // Global reverb effect
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    
    // Global chorus effect for stereo width
    juce::dsp::Chorus<float> chorus;
    
    // Smoothed values for global effects to eliminate zipper noise
    juce::SmoothedValue<float> smoothedReverbRoomSize;
    juce::SmoothedValue<float> smoothedReverbDamping;
    juce::SmoothedValue<float> smoothedReverbWet;
    juce::SmoothedValue<float> smoothedReverbDry;
    juce::SmoothedValue<float> smoothedReverbWidth;
    juce::SmoothedValue<float> smoothedChorusRate;
    juce::SmoothedValue<float> smoothedChorusDepth;
    juce::SmoothedValue<float> smoothedChorusMix;
    
    // Pre-allocated effects buffer to avoid real-time allocations
    juce::AudioBuffer<float> effectsBuffer;
    
    // Device settings for effects re-preparation
    double currentSampleRate = 0.0;
    int currentBlockSize = 0;
    
    // Performance monitoring and optimization
    int performanceMode = 0; // 0=normal, 1=reduced, 2=minimal
    int cpuUsageCounter = 0;
    static constexpr int CPU_MONITOR_INTERVAL = 1000; // Check every 1000 audio blocks
    
    // Main UI components
    juce::FlexBox mainFlexBox;
    juce::FlexBox noteGridFlexBox;
    juce::FlexBox controlFlexBox;
    
    // Note buttons (12 semitones)
    std::array<juce::TextButton, 12> noteButtons;
    std::array<juce::String, 12> noteNames = {
        "C", "C#", "D", "D#", "E", "F", 
        "F#", "G", "G#", "A", "A#", "B"
    };
    
    
    // Main control buttons
    juce::ToggleButton advancedControlsToggle;
    
    
    
    // Waveform segment control
    juce::OwnedArray<juce::TextButton> waveBtns;
    juce::StringArray waves { "Sine", "Square", "Triangle", "Saw" };
    int waveIndex = 0;
    
    
    // Divider component
    juce::Component dividerComponent;
    
    // Panel containers - only one visible at a time
    juce::Component playPanel;          // holds notes/chords/volume/labels/divider
    juce::Viewport advancedVP;
    struct AdvancedPanel : juce::Component {
        std::array<std::unique_ptr<juce::Slider>, 25> sliders; // Added 3 chorus controls
        std::array<std::unique_ptr<juce::Label>, 25>  labels;  // Added 3 chorus controls
        void resized() override {
            auto r = getLocalBounds().reduced(8);
            const int gap = 8;
            const int cols = 3, rows = 9; // Updated for 25 sliders (3x9 = 27, fits 25)

            int colW = (r.getWidth() - (cols - 1) * gap) / cols;
            int rowH = 80; // Fixed height for each row to ensure sliders are visible

            // Simple manual layout for better control
            int sliderIndex = 0;
            for (int row = 0; row < rows && sliderIndex < 25; ++row) {
                for (int col = 0; col < cols && sliderIndex < 25; ++col) {
                    int x = r.getX() + col * (colW + gap);
                    int y = r.getY() + row * (rowH + gap);
                    
                    // Layout slider with label below
                    auto sliderBounds = juce::Rectangle<int>(x, y, colW, rowH - 20);
                    auto labelBounds = juce::Rectangle<int>(x, y + rowH - 18, colW, 18);
                    
                    if (sliderIndex < static_cast<int>(sliders.size())) {
                        sliders[static_cast<size_t>(sliderIndex)]->setBounds(sliderBounds);
                        labels[static_cast<size_t>(sliderIndex)]->setBounds(labelBounds);
                    }
                    
                    sliderIndex++;
                }
            }
        }
    } advancedPanel;
    
    // State
    bool isPlaying = false;
    bool showAdvanced = false;
    bool isAudioInitialized = false;
    bool advancedSlidersCreated = false; // Track lazy loading of advanced sliders
    
    // Audio route change notification handling (simplified)
    void onAudioRouteChanged();
    
    
    // State persistence
    std::unique_ptr<juce::PropertiesFile> propertiesFile;
    
    // Layout constants - responsive design
    static constexpr int MIN_BUTTON_SIZE = 44; // Minimum 44pt for proper hit targets
    static constexpr int MAX_BUTTON_SIZE = 100;
    static constexpr int BUTTON_MARGIN = 6;
    static constexpr int GRID_ROWS = 2;
    static constexpr int GRID_COLS = 6;
    
    // Breakpoints for responsive design
    static constexpr int MIN_WINDOW_WIDTH = 320;   // Minimum width for mobile
    static constexpr int SMALL_PHONE_BREAKPOINT = 480;  // Small phone breakpoint
    static constexpr int MEDIUM_PHONE_BREAKPOINT = 600; // Medium phone breakpoint
    static constexpr int TABLET_BREAKPOINT = 768;  // Tablet breakpoint
    static constexpr int LARGE_TABLET_BREAKPOINT = 1024; // Large tablet breakpoint
    
    void setupFlexBoxLayout();
    void setupNoteButtons();
    void setupAdvancedControlsBasic(); // Only create toggle, defer sliders
    void createAdvancedSliders(); // Lazy load the 25 sliders
    void setupWaveformControls();
    
    void onNoteButtonClicked(int noteIndex);
    void onAdvancedControlsToggle();
    
    // Waveform control
    void setWave(int idx);
    
    void updateAdvancedControls();
    void updateEffectsParameters(); // Update effects parameters when UI controls change
    void updateNoteButtonVisualFeedback();
    
    // Performance optimization methods
    void adjustPerformanceSettings();
    void setPerformanceMode(int mode);
    
    // Responsive design helper methods
    int calculateButtonSize(int availableWidth) const;
    int calculateButtonsPerRow(int availableWidth) const;
    bool isTabletSize() const;
    bool isSmallPhoneSize() const;
    bool isMediumPhoneSize() const;
    bool isLargeTabletSize() const;
    bool isLandscapeOrientation() const;
    int getResponsiveMargin() const;
    void updateButtonSizes();
    void updateResponsiveLayout();
    
    // Initialize audio safely with JUCE defaults and fallback
    void initialiseAudioSafely();
    
    // Haptic feedback helper
    void triggerHapticFeedback();
    
    // Audio debugging
    void toggleTestTone();
    
    // Audio interruption handling
    void onAudioInterruption(void* notification);
    
    // State persistence
    void initializePropertiesFile();
    void saveState();
    void loadState();
    void saveSettings();
    void loadSettings();
    
    // Accessibility
    void setupBasicAccessibilityFocusOrder(); // For note buttons and basic controls
    void setupAdvancedAccessibilityFocusOrder(); // For advanced sliders (deferred)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
