#pragma once

#include <JuceHeader.h>
#include "PadSynthesizer.h"
#include "Theme.h"
#include "LookAndFeelMinimal.h"

// Using the new LookAndFeelMinimal for cleaner, more minimal design

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
    void layoutNotesAndChords(juce::Rectangle<int> bounds);
    
    // Swipe gesture support
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    
    // Keyboard support for debugging
    bool keyPressed(const juce::KeyPress& key) override;

private:
    PadSynthesizer padSynthesizer;
    
    // Custom look and feel
    LookAndFeelMinimal lookAndFeelMinimal;
    
    // Global reverb effect
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    
    // Main UI components
    juce::FlexBox mainFlexBox;
    juce::FlexBox noteGridFlexBox;
    juce::FlexBox chordFlexBox;
    juce::FlexBox controlFlexBox;
    
    // Note buttons (12 semitones)
    std::array<juce::TextButton, 12> noteButtons;
    std::array<juce::String, 12> noteNames = {
        "C", "C#", "D", "D#", "E", "F", 
        "F#", "G", "G#", "A", "A#", "B"
    };
    
    // Chord buttons
    juce::TextButton cMajorButton;
    juce::TextButton cMinorButton;
    juce::TextButton cSus2Button;
    juce::TextButton cSus4Button;
    
    // Main control buttons
    juce::TextButton playStopButton;
    juce::TextButton releaseAllButton;
    juce::ToggleButton advancedControlsToggle;
    
    // UI Feedback components
    juce::Label chordDisplayLabel;
    juce::Label playbackStatusLabel;
    juce::Label activeNotesLabel;
    
    // Frequency control and display
    juce::Label freqReadout;
    juce::Label subLabel;
    
    // Waveform segment control
    juce::OwnedArray<juce::TextButton> waveBtns;
    juce::StringArray waves { "Sine", "Square", "Triangle", "Saw" };
    int waveIndex = 0;
    
    // Volume controls
    juce::Label volLabel;
    juce::Slider masterSlider;
    juce::Slider leftSlider;
    juce::Slider rightSlider;
    
    // Divider component
    juce::Component dividerComponent;
    
    // Panel containers - only one visible at a time
    juce::Component playPanel;          // holds notes/chords/volume/labels/divider
    juce::Viewport advancedVP;
    struct AdvancedPanel : juce::Component {
        std::array<std::unique_ptr<juce::Slider>, 12> sliders;
        std::array<std::unique_ptr<juce::Label>, 12>  labels;
        void resized() override {
            auto r = getLocalBounds().reduced(12);
            const int gap = 12;
            const int cols = 3, rows = 4;

            int colW = (r.getWidth() - (cols - 1) * gap) / cols;
            int rowH = (r.getHeight() - (rows - 1) * gap) / rows;

            juce::Grid g;
            g.autoFlow = juce::Grid::AutoFlow::row;
            for (int c = 0; c < cols; ++c) g.templateColumns.add(juce::Grid::TrackInfo(juce::Grid::Px(colW)));
            for (int rI = 0; rI < rows; ++rI) g.templateRows.add(juce::Grid::TrackInfo(juce::Grid::Px(rowH)));
            g.columnGap = juce::Grid::Px(gap);
            g.rowGap = juce::Grid::Px(gap);

            g.items.clear();
            for (int i = 0; i < (int)sliders.size(); ++i)
                g.items.add(juce::GridItem(*sliders[i]));

            g.performLayout(r);
        }
    } advancedPanel;
    
    // State
    bool isPlaying = false;
    bool showAdvanced = false;
    bool isAudioInitialized = false;
    bool testToneEnabled = false; // For audio debugging
    
    // Audio route change notification handling
    bool audioRouteReady = false;
    void setupAudioRouteNotification();
    void onAudioRouteChanged();
    
    // Frequency control state
    double frequencyHz = 440.0;
    juce::Point<int> dragStart;
    double startFreq = frequencyHz;
    
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
    void setupChordButtons();
    void setupAdvancedControls();
    void setupPlayStopButton();
    void setupUIFeedback();
    void setupFrequencyControls();
    void setupWaveformControls();
    void setupVolumeControls();
    
    void onNoteButtonClicked(int noteIndex);
    void onChordButtonClicked(const std::vector<int>& chord);
    void onPlayStopClicked();
    void onReleaseAllClicked();
    void onAdvancedControlsToggle();
    
    // Frequency and waveform control
    void setFrequency(double hz);
    void setWave(int idx);
    
    void updateAdvancedControls();
    void updateChordDisplay();
    void updateNoteButtonVisualFeedback();
    void updateActiveNotesDisplay();
    juce::String getChordName(const std::vector<int>& activeNotes);
    
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
    void setupAccessibilityFocusOrder();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
