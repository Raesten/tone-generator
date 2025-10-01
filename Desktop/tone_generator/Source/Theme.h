#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace Theme
{
    // Colors - using static const instead of constexpr for JUCE compatibility
    static const juce::Colour bg        = juce::Colour (0xFF111214); // near-black
    static const juce::Colour card      = juce::Colour (0xFF17191C); // darker background for better contrast
    static const juce::Colour accent    = juce::Colour (0xFF2ED1A2); // mint-teal accent
    static const juce::Colour text      = juce::Colour (0xFFEAECEF);
    static const juce::Colour textDim   = juce::Colour (0xFFC3CBD3); // improved contrast for WCAG AA
    static const juce::Colour knobTrack = juce::Colour (0xFF2A2D32);
    static const juce::Colour knobFill  = accent;
    static const juce::Colour btnOff    = juce::Colour (0xFF2A2D32);
    static const juce::Colour btnOn     = accent;

    // Metrics
    static constexpr int   corner = 12;
    static constexpr float gridGap = 12.0f;
    static constexpr int   playHeight = 56;
    static constexpr int   segHeight  = 44; // Minimum 44pt for proper hit targets
    static constexpr int   cardPadding = 16; // Inner padding for card content
    static constexpr int   gridInset = 8;    // Additional inset for grid layouts

    // Typography - using simple Font constructor for compatibility
    inline juce::Font title()     { return juce::Font (26.0f, juce::Font::bold); }
    inline juce::Font readout()   { return juce::Font (44.0f, juce::Font::plain); }
    inline juce::Font label()     { return juce::Font (14.0f, juce::Font::plain); }
    inline juce::Font button()    { return juce::Font (16.0f, juce::Font::bold); }
    
    // Responsive typography helpers
    inline juce::Font title(float scale)     { return juce::Font (26.0f * scale, juce::Font::bold); }
    inline juce::Font readout(float scale)   { return juce::Font (44.0f * scale, juce::Font::plain); }
    inline juce::Font label(float scale)     { return juce::Font (14.0f * scale, juce::Font::plain); }
    inline juce::Font button(float scale)    { return juce::Font (16.0f * scale, juce::Font::bold); }
    
    // Additional theme colors for enhanced UI states
    static const juce::Colour btnHover     = juce::Colour (0xFF3A3D42);
    static const juce::Colour btnPressed   = juce::Colour (0xFF1A1D20);
    static const juce::Colour btnActive    = juce::Colour (0xFF2ED1A2);
    static const juce::Colour btnActiveHover = juce::Colour (0xFF3EE1B2);
    static const juce::Colour border       = juce::Colour (0xFF2A2D32);
    static const juce::Colour shadow       = juce::Colour (0x80000000);
    
    // Status colors
    static const juce::Colour statusPlaying = juce::Colour (0xFF2ED1A2);
    static const juce::Colour statusStopped = juce::Colour (0xFFC3CBD3); // improved contrast for WCAG AA
    static const juce::Colour statusError   = juce::Colour (0xFFFF6B6B);
    static const juce::Colour statusWarning = juce::Colour (0xFFFFD93D);
    
    // Note button states
    static const juce::Colour noteOff       = juce::Colour (0xFF2A2D32);
    static const juce::Colour noteSelected  = juce::Colour (0xFF4A9EFF);
    static const juce::Colour notePlaying   = juce::Colour (0xFF00FF88);
    static const juce::Colour noteHover     = juce::Colour (0xFF3A3D42);
}
