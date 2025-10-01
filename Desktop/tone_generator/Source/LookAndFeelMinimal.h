// LookAndFeelMinimal.h
#pragma once
#include "Theme.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct LookAndFeelMinimal : juce::LookAndFeel_V4
{
    LookAndFeelMinimal()
    {
        setColour (juce::ResizableWindow::backgroundColourId, Theme::bg);
        setColour (juce::Label::textColourId, Theme::text);
        setColour (juce::TextButton::textColourOnId, Theme::bg);
        setColour (juce::TextButton::textColourOffId, Theme::text);
        setColour (juce::Slider::textBoxTextColourId, Theme::text);
        setColour (juce::Slider::backgroundColourId, Theme::knobTrack);
        setColour (juce::Slider::thumbColourId, Theme::accent);
        setColour (juce::Slider::trackColourId, Theme::knobFill);
    }

    // Flat, rounded TextButtons
    void drawButtonBackground (juce::Graphics& g, juce::Button& b,
                               const juce::Colour&, bool, bool) override
    {
        auto r = b.getLocalBounds().toFloat();
        
        // Ensure we have valid dimensions
        if (r.getWidth() <= 0 || r.getHeight() <= 0)
            return;
            
        auto on = b.getToggleState();
        g.setColour (on ? Theme::btnOn : Theme::btnOff);
        g.fillRoundedRectangle (r, Theme::corner);

        // subtle border
        g.setColour (Theme::btnOff.darker(0.8f));
        g.drawRoundedRectangle (r.reduced(0.5f), Theme::corner, 1.0f);
    }

    // Segmented control: give pressed button an inner glow
    void drawButtonText (juce::Graphics& g, juce::TextButton& b,
                         bool, bool) override
    {
        g.setFont (Theme::button());
        g.setColour (b.getToggleState() ? Theme::bg : Theme::text);
        g.drawFittedText (b.getButtonText(), b.getLocalBounds(), juce::Justification::centred, 1);
    }

    // Flat linear sliders
    void drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                           float pos, float, float, const juce::Slider::SliderStyle, juce::Slider&) override
    {
        // Ensure we have valid dimensions
        if (w <= 0 || h <= 12)
            return;
            
        // Clamp pos to valid range [0.0, 1.0]
        pos = juce::jlimit (0.0f, 1.0f, pos);
        
        auto track = juce::Rectangle<float> ((float) x + w * 0.35f, (float) y + 6, 4.0f, (float) h - 12);
        
        // Ensure track has valid dimensions
        if (track.getWidth() <= 0 || track.getHeight() <= 0)
            return;
            
        g.setColour (Theme::knobTrack);
        g.fillRoundedRectangle (track, 2.0f);

        // Calculate fill height safely
        auto fillHeight = track.getHeight() * (1.0f - pos);
        fillHeight = juce::jlimit (0.0f, track.getHeight(), fillHeight);
        
        auto fill = track.removeFromBottom (fillHeight);
        g.setColour (Theme::knobFill);
        g.fillRoundedRectangle (fill, 2.0f);
    }

    // Labels on dark cards
    void drawLabel (juce::Graphics& g, juce::Label& l) override
    {
        g.fillAll (juce::Colours::transparentBlack);
        g.setColour (Theme::text);
        g.setFont (l.getFont());
        g.drawFittedText (l.getText(), l.getLocalBounds(), l.getJustificationType(), 1);
    }
};
