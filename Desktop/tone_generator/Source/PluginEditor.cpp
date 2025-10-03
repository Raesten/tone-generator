/*
  ==============================================================================



  ==============================================================================
*/

#ifdef __APPLE__ 

#include "PluginProcessor.h"
#include "PluginEditor.h"

#elif defined _WIN32 || defined _WIN64

#include "PluginProcessor.h"
#include "PluginEditor.h"

#endif

//==============================================================================
JuceDemoPluginAudioProcessorEditor::JuceDemoPluginAudioProcessorEditor (JuceDemoPluginAudioProcessor& owner)
    : juce::AudioProcessorEditor (owner),
      midiKeyboard (owner.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
	//setLookAndFeel(&otherLookAndFeel);
    // add the midi keyboard component..
    addAndMakeVisible (midiKeyboard);

	// const int readBoxHeight = 15;
	// const int readBoxWidth = 50;
	juce::Colour labelColour = juce::Colour(1.0f, 0.0f, 0.0f);
	juce::Font labelFont = juce::Font(juce::FontOptions("Times New Roman", 20.0f, juce::Font::italic));

	setupSliders();

	//combo boxes
	comboBoxes[OSCIL_TYPE].addItem("Sine Osc", 1);
	comboBoxes[OSCIL_TYPE].addItem("Square Osc", 2);
	comboBoxes[OSCIL_TYPE].addItem("Saw Osc", 3);
	comboBoxes[OSCIL_TYPE].addItem("Noise", 4);

	comboBoxes[OSCIL_TYPE].setSelectedItemIndex(2);

	comboBoxes[LFO_TYPE].addItem("Amplitude", 1);
	//comboBoxes[LFO_TYPE].addItem("Frequency", 2);
	comboBoxes[LFO_TYPE].setSelectedItemIndex(0);

	for (int i = 0; i < NUM_COMBO_BOX; i++)
	{
		addAndMakeVisible(&comboBoxes[static_cast<size_t>(i)]);
		comboBoxes[static_cast<size_t>(i)].addListener(this);
	}

	//labels
	for (int i = 0; i < NUM_LABELS; i++)
	{
		labels[static_cast<size_t>(i)].setText(labelText.at(i), juce::dontSendNotification);
		labels[static_cast<size_t>(i)].setJustificationType(juce::Justification::centred);
		labels[static_cast<size_t>(i)].setFont(labelFont);
		labels[static_cast<size_t>(i)].setColour(juce::Label::textColourId, labelColour);
		addAndMakeVisible(&labels[static_cast<size_t>(i)]);
	}

	//Name and oscil type
	nameLabel.setText("GENERATOR \nCreated by Chris Wratt", juce::dontSendNotification);
	nameLabel.setJustificationType(juce::Justification::centred);
	nameLabel.setColour(juce::Label::textColourId, labelColour);
	nameLabel.setFont(labelFont);
	addAndMakeVisible(&nameLabel);

    // set resize limits for this plug-in
    setResizeLimits (700, 400, 1000, 600);

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (owner.lastUIWidth,
             owner.lastUIHeight);
}

void JuceDemoPluginAudioProcessorEditor::setupSliders()
{
	for (int i = 0; i < NUM_VERT_DELAY; i++)
	{
		vertSlidersDelay[static_cast<size_t>(i)].setSliderStyle(juce::Slider::LinearVertical);
		vertSlidersDelay[static_cast<size_t>(i)].setRange(0.0, 1000.0, 1.0);
		vertSlidersDelay[static_cast<size_t>(i)].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		vertSlidersDelay[static_cast<size_t>(i)].setPopupDisplayEnabled(true, false, this);
		vertSlidersDelay[static_cast<size_t>(i)].setTextValueSuffix(" MS");
		vertSlidersDelay[static_cast<size_t>(i)].addListener(this);
		addAndMakeVisible(&vertSlidersDelay[static_cast<size_t>(i)]);
	}

	vertSlidersDelay[VOLUME_A_SLIDER].setValue(volumeDefaultA);
	vertSlidersDelay[VOLUME_D_SLIDER].setValue(volumeDefaultD);
	vertSlidersDelay[VOLUME_R_SLIDER].setValue(volumeDefaultR);

	vertSlidersDelay[FILTER_A_SLIDER].setValue(filterDefaultA);
	vertSlidersDelay[FILTER_D_SLIDER].setValue(filterDefaultD);
	vertSlidersDelay[FILTER_R_SLIDER].setValue(filterDefaultR);

	for (int i = 0; i < NUM_VERT_AMP; i++)
	{
		vertSlidersAmp[static_cast<size_t>(i)].setSliderStyle(juce::Slider::LinearVertical);
		vertSlidersAmp[static_cast<size_t>(i)].setRange(0.0, 1.0, 0.001);
		vertSlidersAmp[static_cast<size_t>(i)].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		vertSlidersAmp[static_cast<size_t>(i)].setPopupDisplayEnabled(true, false, this);
		vertSlidersAmp[static_cast<size_t>(i)].setTextValueSuffix(" Amp");
		vertSlidersAmp[static_cast<size_t>(i)].addListener(this);
		addAndMakeVisible(&vertSlidersAmp[static_cast<size_t>(i)]);
	}

	vertSlidersAmp[VOLUME_S_SLIDER].setValue(volumeDefaultS);
	vertSlidersAmp[FILTER_S_SLIDER].setValue(filterDefaultS);

	for (int i = 0; i < NUM_POT_SLIDERS; i++)
	{
		potSliders[static_cast<size_t>(i)].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		potSliders[static_cast<size_t>(i)].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		potSliders[static_cast<size_t>(i)].setPopupDisplayEnabled(true, false, this);
		potSliders[static_cast<size_t>(i)].setTextValueSuffix(" Mix");
		potSliders[static_cast<size_t>(i)].addListener(this);
		addAndMakeVisible(&potSliders[static_cast<size_t>(i)]);
	}

	potSliders[REVERB_MIX_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[REVERB_MIX_SLIDER].setValue(0.2);
	potSliders[REVERB_LEN_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[REVERB_LEN_SLIDER].setValue(0.5);
	potSliders[DELAY_MIX_SLIDER].setRange(0.0, 1.0, 0.001);			potSliders[DELAY_MIX_SLIDER].setValue(0.0);
	potSliders[DELAY_LEN_SLIDER].setRange(0.0, 3.0, 0.001);			potSliders[DELAY_LEN_SLIDER].setValue(0.4);
	potSliders[DELAY_FEEDBACK_SLIDER].setRange(0.0, 1.0, 0.001);	potSliders[DELAY_FEEDBACK_SLIDER].setValue(0.5);
	potSliders[CHORUS_MIX_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[CHORUS_MIX_SLIDER].setValue(0.1);
	potSliders[CHORUS_MODAMT_SLIDER].setRange(0.0, 1.0, 0.001);		potSliders[CHORUS_MODAMT_SLIDER].setValue(0.01);
	potSliders[CHORUS_MOD_SPEED_SLIDER].setRange(0.0, 30.0, 0.001); potSliders[CHORUS_MOD_SPEED_SLIDER].setValue(0.1);
	potSliders[LFO_RATE_SLIDER].setRange(0.0, 100.0, 0.001);		potSliders[LFO_RATE_SLIDER].setValue(20.0);
	potSliders[LFO_RATE_SLIDER].setTextValueSuffix(" Hz");			potSliders[LFO_RATE_SLIDER].setSkewFactor(0.5);
	potSliders[LFO_AMP_SLIDER].setRange(0.0, 1.0, 0.001);			potSliders[LFO_AMP_SLIDER].setValue(0.1);
	potSliders[LFO_AMP_SLIDER].setTextValueSuffix("");
	potSliders[FILTER_START_SLIDER].setRange(20.0, 20000.0, 1.0);	potSliders[FILTER_START_SLIDER].setValue(filterStartDefault);
	potSliders[FILTER_START_SLIDER].setTextValueSuffix(" Hz");
	potSliders[FILTER_END_SLIDER].setRange(20.0, 20000.0, 1.0);		potSliders[FILTER_END_SLIDER].setValue(filterEndDefault);
	potSliders[FILTER_END_SLIDER].setTextValueSuffix(" Hz");

}

JuceDemoPluginAudioProcessorEditor::~JuceDemoPluginAudioProcessorEditor()
{
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
	const juce::Colour colourOne = juce::Colours::lightblue;
	const juce::Colour colourTwo = juce::Colours::white;
	juce::Rectangle<int> area = getLocalBounds();

	juce::Rectangle<int> top = area.removeFromTop(area.getHeight() / 2);
	juce::Rectangle<int> topLeft = top.removeFromLeft(top.getWidth() / 3);
	juce::Rectangle<int> topMid = top.removeFromLeft(top.getWidth() / 2);
	juce::Rectangle<int> topRight = top;

	juce::Rectangle<int> bottomLeft = area.removeFromLeft(area.getWidth() / 3);
	juce::Rectangle<int> bottomMid = area.removeFromLeft(area.getWidth() / 2);
	juce::Rectangle<int> bottomRight = area;

	g.setColour(colourOne);
	g.fillRect(topLeft);
	g.setColour(colourTwo);
	g.fillRect(topMid);
	g.setColour(colourOne);
	g.fillRect(topRight);

	g.setColour(colourTwo);
	g.fillRect(bottomLeft);
	g.setColour(colourOne);
	g.fillRect(bottomMid);
	g.setColour(colourTwo);
	g.fillRect(bottomRight);
}

void JuceDemoPluginAudioProcessorEditor::resized()
{
	const int labelHeightRatio = 5;
	const float comboBoxRatio = 3.0;
	const int comboBoxMargin = 4;

	//general dividing into rects
	juce::Rectangle<int> fullArea(getLocalBounds());
	juce::Rectangle<int> halfAreaTop = fullArea.removeFromTop(fullArea.getHeight() / 2);
	{
		juce::Rectangle<int> topLeftThirdRect = halfAreaTop.removeFromLeft(halfAreaTop.getWidth() / 3);
		nameLabel.setBounds(topLeftThirdRect.removeFromTop(topLeftThirdRect.getHeight() / 2));
		comboBoxes[OSCIL_TYPE].setBounds(topLeftThirdRect.removeFromTop(static_cast<int>(topLeftThirdRect.getHeight() / comboBoxRatio)).reduced(comboBoxMargin));
	}

	//ADSR
	juce::Rectangle<int> topMiddleRect = halfAreaTop.removeFromLeft(halfAreaTop.getWidth() / 2);
	{
		labels[VOLUME_ADSR_LABEL].setBounds(topMiddleRect.removeFromTop(topMiddleRect.getHeight() / labelHeightRatio));
		vertSlidersDelay[VOLUME_A_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth() / 4));
		vertSlidersDelay[VOLUME_D_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth() / 3));
		vertSlidersAmp[VOLUME_S_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth() / 2));
		vertSlidersDelay[VOLUME_R_SLIDER].setBounds(topMiddleRect.removeFromLeft(topMiddleRect.getWidth()));
	}

	//LFO
	juce::Rectangle<int> topRightRect = halfAreaTop;
	{
		juce::Rectangle<int> top = topRightRect.removeFromTop(topRightRect.getHeight() / 2);
		juce::Rectangle<int> topBottom = top.removeFromBottom(top.getHeight() / 2);
		/*juce::Rectangle<int> placeHolderRect1 = */top.removeFromTop(static_cast<int>(top.getHeight() / (comboBoxRatio / 2)));
		comboBoxes[LFO_TYPE].setBounds(topBottom.removeFromTop(static_cast<int>(topBottom.getHeight() / (comboBoxRatio / 2))).reduced(comboBoxMargin));
		juce::Rectangle<int> bottomLeft = topRightRect.removeFromLeft(topRightRect.getWidth() / 2);
		labels[LFO_RATE_LABEL].setBounds(bottomLeft.removeFromTop(bottomLeft.getHeight() / labelHeightRatio));
		potSliders[LFO_RATE_SLIDER].setBounds(bottomLeft);
		labels[LFO_AMP_LABEL].setBounds(topRightRect.removeFromTop(topRightRect.getHeight() / labelHeightRatio));
		potSliders[LFO_AMP_SLIDER].setBounds(topRightRect.removeFromLeft(topRightRect.getWidth()));
	}

	//Filter
	juce::Rectangle<int> bottomLeftRect = fullArea.removeFromLeft(fullArea.getWidth() / 3);
	{
		juce::Rectangle<int> top = bottomLeftRect.removeFromTop(bottomLeftRect.getHeight() / 2);
		/*juce::Rectangle<int> placeHolderRect2 = */top.removeFromTop(static_cast<int>(top.getHeight() / comboBoxRatio));
		juce::Rectangle<int> bottomLeft = bottomLeftRect.removeFromLeft(bottomLeftRect.getWidth() / 2);
		labels[FILTER_START_FREQ].setBounds(bottomLeft.removeFromTop(bottomLeft.getHeight() / labelHeightRatio));
		potSliders[FILTER_START_SLIDER].setBounds(bottomLeft);
		labels[FILTER_END_FREQ].setBounds(bottomLeftRect.removeFromTop(bottomLeftRect.getHeight() / labelHeightRatio));
		potSliders[FILTER_END_SLIDER].setBounds(bottomLeftRect);
	}

	//Filter ADSR
	juce::Rectangle<int> bottompMiddleThirdRect = fullArea.removeFromLeft(fullArea.getWidth() / 2);
	{
		labels[FILTER_ADSR_LABEL].setBounds(bottompMiddleThirdRect.removeFromTop(bottompMiddleThirdRect.getHeight() / labelHeightRatio));
		vertSlidersDelay[FILTER_A_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth() / 4));
		vertSlidersDelay[FILTER_D_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth() / 3));
		vertSlidersAmp[FILTER_S_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth() / 2));
		vertSlidersDelay[FILTER_R_SLIDER].setBounds(bottompMiddleThirdRect.removeFromLeft(bottompMiddleThirdRect.getWidth()));
	}

	//Effects
	juce::Rectangle<int> bottomRightRect = fullArea;
	{
		juce::Rectangle<int> topThird = bottomRightRect.removeFromTop(bottomRightRect.getHeight() / 3);
		juce::Rectangle<int> topThirdLeft = topThird.removeFromLeft(topThird.getWidth() / 3);
		/*juce::Rectangle<int> topThirdMid = */topThird.removeFromLeft(topThird.getWidth() / 2);
		juce::Rectangle<int> topThirdRight = topThird;
		juce::Rectangle<int> middleThird = bottomRightRect.removeFromTop(bottomRightRect.getHeight() / 2);
		/*juce::Rectangle<int> midThirdLeft = */middleThird.removeFromLeft(middleThird.getWidth() / 3);
		/*juce::Rectangle<int> midThirdMid = */middleThird.removeFromLeft(middleThird.getWidth() / 2);
		juce::Rectangle<int> midThirdRight = middleThird;
		(void)midThirdRight; // Suppress unused variable warning
		juce::Rectangle<int> bottomThird = bottomRightRect;
		juce::Rectangle<int> bottomThirdLeft = bottomThird.removeFromLeft(bottomThird.getWidth() / 3);
		juce::Rectangle<int> bottomThirdMid = bottomThird.removeFromLeft(bottomThird.getWidth() / 2);
		juce::Rectangle<int> bottomThirdRight = bottomThird;

		labels[REVERB_LEN_LABEL].setBounds(topThirdLeft.removeFromTop(topThirdLeft.getHeight() / labelHeightRatio));
		labels[REVERB_MIX_LABEL].setBounds(topThirdRight.removeFromTop(topThirdRight.getHeight() / labelHeightRatio));
		potSliders[REVERB_LEN_SLIDER].setBounds(topThirdLeft);
		potSliders[REVERB_MIX_SLIDER].setBounds(topThirdRight);

		//labels[DELAY_LEN_LABEL].setBounds(midThirdLeft.removeFromTop(midThirdLeft.getHeight() / labelHeightRatio));
		//labels[DELAY_FEEDBACK_LABEL].setBounds(midThirdMid.removeFromTop(midThirdMid.getHeight() / labelHeightRatio));
		//labels[DELAY_MIX_LABEL].setBounds(midThirdRight.removeFromTop(midThirdRight.getHeight() / labelHeightRatio));
		//potSliders[DELAY_LEN_SLIDER].setBounds(midThirdLeft);
		//potSliders[DELAY_FEEDBACK_SLIDER].setBounds(midThirdMid);
		//potSliders[DELAY_MIX_SLIDER].setBounds(midThirdRight);

		labels[CHORUS_MODAMT_LABEL].setBounds(bottomThirdLeft.removeFromTop(bottomThirdLeft.getHeight() / labelHeightRatio));
		labels[CHORUS_MOD_SPEED_LABEL].setBounds(bottomThirdMid.removeFromTop(bottomThirdMid.getHeight() / labelHeightRatio));
		labels[CHORUS_MIX_LABEL].setBounds(bottomThirdRight.removeFromTop(bottomThirdRight.getHeight() / labelHeightRatio));
		potSliders[CHORUS_MODAMT_SLIDER].setBounds(bottomThirdLeft);
		potSliders[CHORUS_MOD_SPEED_SLIDER].setBounds(bottomThirdMid);
		potSliders[CHORUS_MIX_SLIDER].setBounds(bottomThirdRight);
	}
}

//==============================================================================
void JuceDemoPluginAudioProcessorEditor::timerCallback()
{
}

void JuceDemoPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
				if (slider == &vertSlidersDelay[VOLUME_A_SLIDER]) ParameterHolder::inst().parameters[VOLUME_A_PARAM].store(static_cast<float>(slider->getValue() / 1000.0));
		else if (slider == &vertSlidersDelay[VOLUME_D_SLIDER]) ParameterHolder::inst().parameters[VOLUME_D_PARAM].store(static_cast<float>(slider->getValue() / 1000.0));
		else if (slider == &vertSlidersAmp[VOLUME_S_SLIDER]) ParameterHolder::inst().parameters[VOLUME_S_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &vertSlidersDelay[VOLUME_R_SLIDER]) ParameterHolder::inst().parameters[VOLUME_R_PARAM].store(static_cast<float>(slider->getValue() / 1000.0));
		else if (slider == &vertSlidersDelay[FILTER_A_SLIDER]) ParameterHolder::inst().parameters[FILTER_A_PARAM].store(static_cast<float>(slider->getValue() / 1000.0));
		else if (slider == &vertSlidersDelay[FILTER_D_SLIDER]) ParameterHolder::inst().parameters[FILTER_D_PARAM].store(static_cast<float>(slider->getValue() / 1000.0));
		else if (slider == &vertSlidersAmp[FILTER_S_SLIDER]) ParameterHolder::inst().parameters[FILTER_S_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &vertSlidersDelay[FILTER_R_SLIDER]) ParameterHolder::inst().parameters[FILTER_R_PARAM].store(static_cast<float>(slider->getValue() / 1000.0));
		else if (slider == &potSliders[REVERB_MIX_SLIDER]) ParameterHolder::inst().parameters[REVERB_MIX_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[REVERB_LEN_SLIDER]) ParameterHolder::inst().parameters[REVERB_LEN_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[DELAY_MIX_SLIDER]) ParameterHolder::inst().parameters[DELAY_MIX_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[DELAY_LEN_SLIDER]) ParameterHolder::inst().parameters[DELAY_LEN_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[DELAY_FEEDBACK_LABEL]) ParameterHolder::inst().parameters[DELAY_FEEDBACK_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[CHORUS_MIX_SLIDER]) ParameterHolder::inst().parameters[CHORUS_MIX_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[CHORUS_MODAMT_SLIDER]) ParameterHolder::inst().parameters[CHORUS_MODAMT_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[CHORUS_MOD_SPEED_SLIDER]) ParameterHolder::inst().parameters[CHORUS_MOD_SPEED_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[LFO_RATE_SLIDER]) ParameterHolder::inst().parameters[LFO_RATE_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[LFO_AMP_SLIDER]) ParameterHolder::inst().parameters[LFO_AMP_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[FILTER_START_SLIDER]) ParameterHolder::inst().parameters[FILTER_START_PARAM].store(static_cast<float>(slider->getValue()));
		else if (slider == &potSliders[FILTER_END_SLIDER]) ParameterHolder::inst().parameters[FILTER_END_PARAM].store(static_cast<float>(slider->getValue()));
}

void JuceDemoPluginAudioProcessorEditor::comboBoxChanged (juce::ComboBox* comboBox)
{
	if (comboBox == &comboBoxes[OSCIL_TYPE])
	{

		switch (comboBox->getSelectedItemIndex())
		{
		case(OSC_SINE):
			ParameterHolder::inst().currentOsc = OSC_SINE;
			break;
		case(OSC_SQUARE):
			ParameterHolder::inst().currentOsc = OSC_SQUARE;
			break;
		case(OSC_SAW):
			ParameterHolder::inst().currentOsc = OSC_SAW;
			break;
		case(OSC_NOISE):
			ParameterHolder::inst().currentOsc = OSC_NOISE;
			break;
		}
	}
}
