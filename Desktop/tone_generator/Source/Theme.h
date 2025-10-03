#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace Theme
{
    // ============================================================================
    // SEMANTIC COLOR TOKENS - Design System Implementation
    // ============================================================================
    
    // Background System
    static const juce::Colour bgCanvasTop     = juce::Colour (0xFF0F1114); // #0F1114
    static const juce::Colour bgCanvasBottom  = juce::Colour (0xFF171A1E); // #171A1E
    
    // Key Tiles - Natural Keys (C, D, E, F, G, A, B)
    static const juce::Colour keyNaturalBaseTop    = juce::Colour (0xFF2B2F35); // #2B2F35
    static const juce::Colour keyNaturalBaseBottom = juce::Colour (0xFF353A41); // #353A41
    
    // Key Tiles - Sharp Keys (C#, D#, F#, G#, A#) - no blue tints
    static const juce::Colour keySharpBaseTop      = juce::Colour (0xFF2B2F35); // #2B2F35 (same as natural, no blue)
    static const juce::Colour keySharpBaseBottom   = juce::Colour (0xFF353A41); // #353A41 (same as natural, no blue)
    
    // Key Borders & Highlights
    static const juce::Colour keyBorder            = juce::Colour (0x0FFFFFFF); // rgba(255,255,255,0.06) - 1px hairline
    static const juce::Colour keyInnerTopHighlight = juce::Colour (0x0FFFFFFF); // rgba(255,255,255,0.06) - inset top 15-20%
    static const juce::Colour keyInnerBottomShade  = juce::Colour (0x2E000000); // rgba(0,0,0,0.18) - inset bottom 15-20%
    
    // Text System
    static const juce::Colour textPrimary = juce::Colour (0xFFE9EDF2); // #E9EDF2 - primary text
    static const juce::Colour textDim     = juce::Colour (0xFFB8C0C9); // #B8C0C9 - dim text
    
    // Accent System (feedback & rings)
    static const juce::Colour accentCool = juce::Colour (0xFF6EA8FF);  // #6EA8FF - focus/active rings
    static const juce::Colour accentGlow = juce::Colour (0x596EA8FF);  // rgba(110,168,255,0.35)
    
    // Pressed States - Orange-tinted for all keys (no blue)
    static const juce::Colour keyPressedNaturalTop    = juce::Colour (0xFF3A414A); // #3A414A - brighter and warmer
    static const juce::Colour keyPressedNaturalBottom = juce::Colour (0xFF2E343B); // #2E343B - increased contrast
    static const juce::Colour keyPressedSharpTop      = juce::Colour (0xFF3A414A); // #3A414A - same as natural (no blue)
    static const juce::Colour keyPressedSharpBottom   = juce::Colour (0xFF2E343B); // #2E343B - same as natural (no blue)
    
    // Accent Flash Colors
    static const juce::Colour accentFlashNatural = juce::Colour (0x80FF8A34); // #FF8A34 @ 50% opacity - warm orange (more visible)
    static const juce::Colour accentFlashSharp   = juce::Colour (0x386EA8FF); // #6EA8FF @ 22% opacity - cool blue
    
    // Focus Ring Colors
    static const juce::Colour focusRingNatural = juce::Colour (0x80FF8A34); // #FF8A34 @ 50% opacity (more visible)
    static const juce::Colour focusRingSharp   = juce::Colour (0x596EA8FF); // #6EA8FF @ 35% opacity
    
    // Pressed Shadow Colors
    static const juce::Colour shadowPressedOuter = juce::Colour (0x5C000000); // rgba(0,0,0,0.36) - reduced shadow
    static const juce::Colour shadowPressedInnerTop = juce::Colour (0x1AFFFFFF); // rgba(255,255,255,0.10) - top inset
    static const juce::Colour shadowPressedInnerBottom = juce::Colour (0x2E000000); // rgba(0,0,0,0.18) - bottom inset
    
    // Text Enhancement
    static const juce::Colour textBrightenOverlay = juce::Colour (0x1AFFFFFF); // White overlay @ 10% for text brightening
    static const juce::Colour sharpGlyphTint = juce::Colour (0xFF6EA8FF); // #6EA8FF for sharp "#" glyph when pressed
    
    // Disabled States
    static const juce::Colour keyDisabledFill = juce::Colour (0xFF24272C); // #24272C
    static const juce::Colour textDisabled    = juce::Colour (0xFF6C7580); // #6C7580
    
    // ============================================================================
    // SHADOW SYSTEM - Layered Elevation
    // ============================================================================
    
    // Shadow Colors
    static const juce::Colour shadowOuter    = juce::Colour (0x66000000); // rgba(0,0,0,0.40)
    static const juce::Colour shadowOuterHover = juce::Colour (0x75000000); // rgba(0,0,0,0.46)
    static const juce::Colour shadowOuterPressed = juce::Colour (0x5C000000); // rgba(0,0,0,0.36)
    static const juce::Colour shadowEdge     = juce::Colour (0x0AFFFFFF); // rgba(255,255,255,0.04)
    static const juce::Colour shadowInnerTop = juce::Colour (0x0FFFFFFF); // rgba(255,255,255,0.06)
    static const juce::Colour shadowInnerBottom = juce::Colour (0x1A000000); // rgba(0,0,0,0.10)
    
    // ============================================================================
    // LAYOUT METRICS
    // ============================================================================
    
    // Key Tile Metrics
    static constexpr int keyCornerRadius = 18;  // 16-18px corner radius
    static constexpr int keyBorderWidth = 1;    // 1px border
    static constexpr int keyMinHitSize = 44;    // Minimum 44×44px hit target
    
    // Grid System
    static constexpr int pageMargin = 24;       // 24-28px page margins
    static constexpr int colGap = 20;           // 20-24px column gaps
    static constexpr int rowGap = 20;           // 20-24px row gaps
    
    // Legacy metrics for compatibility
    static constexpr int   corner = keyCornerRadius;
    static constexpr float gridGap = colGap;
    static constexpr int   playHeight = 56;
    static constexpr int   segHeight  = 44;
    static constexpr int   cardPadding = 16;
    static constexpr int   gridInset = 8;
    
    // ============================================================================
    // TYPOGRAPHY SYSTEM
    // ============================================================================
    
    // Typography - using deprecated Font constructor (warnings suppressed)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    
    // Key Tile Labels
    inline juce::Font keyLabel() { return juce::Font (19.0f, juce::Font::bold); } // 18-20px, SemiBold (600)
    
    // Legacy typography for compatibility
    inline juce::Font title()     { return juce::Font (26.0f, juce::Font::bold); }
    inline juce::Font readout()   { return juce::Font (44.0f, juce::Font::plain); }
    inline juce::Font label()     { return juce::Font (14.0f, juce::Font::plain); }
    inline juce::Font button()    { return juce::Font (16.0f, juce::Font::bold); }
    
    // Responsive typography helpers
    inline juce::Font title(float scale)     { return juce::Font (26.0f * scale, juce::Font::bold); }
    inline juce::Font readout(float scale)   { return juce::Font (44.0f * scale, juce::Font::plain); }
    inline juce::Font label(float scale)     { return juce::Font (14.0f * scale, juce::Font::plain); }
    inline juce::Font button(float scale)    { return juce::Font (16.0f * scale, juce::Font::bold); }
    inline juce::Font keyLabel(float scale)  { return juce::Font (19.0f * scale, juce::Font::bold); }
    
    #pragma GCC diagnostic pop
    
    // ============================================================================
    // LEGACY COMPATIBILITY COLORS
    // ============================================================================
    
    // Backward compatibility aliases
    static const juce::Colour bg        = bgCanvasTop;
    static const juce::Colour card      = bgCanvasBottom;
    static const juce::Colour accent    = accentCool;
    static const juce::Colour text      = textPrimary;
    static const juce::Colour knobTrack = keyNaturalBaseTop;
    static const juce::Colour knobFill  = accentCool;
    static const juce::Colour btnOff    = keyNaturalBaseTop;
    static const juce::Colour btnOn     = accentCool;
    
    // Additional theme colors for enhanced UI states
    static const juce::Colour btnHover     = juce::Colour (0xFF3A3D42);
    static const juce::Colour btnPressed   = juce::Colour (0xFF1A1D20);
    static const juce::Colour btnActive    = accentCool;
    static const juce::Colour btnActiveHover = juce::Colour (0xFF3EE1B2);
    static const juce::Colour border       = keyBorder;
    static const juce::Colour shadow       = shadowOuter;
    
    // Status colors
    static const juce::Colour statusPlaying = accentCool;
    static const juce::Colour statusStopped = textDim;
    static const juce::Colour statusError   = juce::Colour (0xFFFF6B6B);
    static const juce::Colour statusWarning = juce::Colour (0xFFFFD93D);
    
    // Note button states - updated to use new color system
    static const juce::Colour noteOff       = keyNaturalBaseTop;
    static const juce::Colour noteSelected  = accentCool;
    static const juce::Colour notePlaying   = juce::Colour (0xFF00FF88);
    static const juce::Colour noteHover     = juce::Colour (0xFF3A3D42);
    
    // ============================================================================
    // UTILITY FUNCTIONS
    // ============================================================================
    
    // Check if a note is a sharp
    inline bool isSharpNote(const juce::String& noteName) {
        return noteName.contains("#") || noteName.contains("♯");
    }
    
    // Get appropriate key colors based on note type
    inline juce::Colour getKeyTopColor(bool isSharp, bool isPressed = false) {
        if (isPressed) {
            return isSharp ? keyPressedSharpTop : keyPressedNaturalTop;
        }
        return isSharp ? keySharpBaseTop : keyNaturalBaseTop;
    }
    
    inline juce::Colour getKeyBottomColor(bool isSharp, bool isPressed = false) {
        if (isPressed) {
            return isSharp ? keyPressedSharpBottom : keyPressedNaturalBottom;
        }
        return isSharp ? keySharpBaseBottom : keyNaturalBaseBottom;
    }
}
