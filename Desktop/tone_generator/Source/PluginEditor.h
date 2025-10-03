/*
  ==============================================================================


  ==============================================================================
*/

#pragma once

#ifdef __APPLE__ 

#include "PluginProcessor.h"
#include <JuceHeader.h>

#elif defined _WIN32 || defined _WIN64

#include "PluginProcessor.h"
#include "..\JuceLibraryCode\JuceHeader.h"

#endif



class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
	OtherLookAndFeel();
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
};

//==============================================================================
/** This is the editor component that our filter will display.
*/
class JuceDemoPluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            private juce::Timer,
											private juce::Slider::Listener,
                                            private juce::ComboBox::Listener
{
public:
    JuceDemoPluginAudioProcessorEditor (JuceDemoPluginAudioProcessor&);
    ~JuceDemoPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:

    juce::MidiKeyboardComponent midiKeyboard;

    //==============================================================================
    JuceDemoPluginAudioProcessor& getProcessor() const
    {
        return static_cast<JuceDemoPluginAudioProcessor&> (processor);
    }
    void sliderValueChanged (juce::Slider* slider) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;
	void setupSliders();
    
	std::map<int, std::string> labelText =
	{
		{ VOLUME_ADSR_LABEL, "Envelope \n(volume)" },{ FILTER_ADSR_LABEL, "Envelope \n(Filter)" },
		{ REVERB_LEN_LABEL, "Verb Size" },{ REVERB_MIX_LABEL, "Verb Mix" },{ DELAY_LEN_LABEL, "Length" },
		{ DELAY_FEEDBACK_LABEL, "Feedback" },{ DELAY_MIX_LABEL, "Mix" },{ CHORUS_MODAMT_LABEL, "Chorus Amount" },
		{ CHORUS_MOD_SPEED_LABEL, "chorus speed" },{ CHORUS_MIX_LABEL, "chorus Mix" }, { LFO_RATE_LABEL, "LFO rate" },
		{ LFO_AMP_LABEL, "LFO amp" },{ FILTER_START_FREQ, "Filter mod1" },{ FILTER_END_FREQ, "Filter mod2" }
	};

	std::array<juce::Slider, NUM_VERT_DELAY> vertSlidersDelay;
	std::array<juce::Slider, NUM_VERT_AMP> vertSlidersAmp;
	std::array<juce::Slider, NUM_POT_SLIDERS> potSliders;
	std::array<juce::ComboBox, NUM_COMBO_BOX> comboBoxes;
	std::array<juce::Label, NUM_LABELS> labels;

    juce::LookAndFeel_V4 colourSchemeOne;
	juce::Label nameLabel;
	OtherLookAndFeel otherLookAndFeel;
};
