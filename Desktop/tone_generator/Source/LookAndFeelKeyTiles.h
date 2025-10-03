// LookAndFeelKeyTiles.h
#pragma once
#include "Theme.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct LookAndFeelKeyTiles : juce::LookAndFeel_V4
{
    LookAndFeelKeyTiles()
    {
        // Set up base colors
        setColour (juce::ResizableWindow::backgroundColourId, Theme::bgCanvasTop);
        setColour (juce::Label::textColourId, Theme::textPrimary);
        setColour (juce::TextButton::textColourOnId, Theme::textPrimary);
        setColour (juce::TextButton::textColourOffId, Theme::textPrimary);
    }
    
    // Suppress unused parameter warnings
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    // ============================================================================
    // KEY TILE BUTTON RENDERING
    // ============================================================================
    
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour&, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        
        // Ensure we have valid dimensions
        if (bounds.getWidth() <= 0 || bounds.getHeight() <= 0)
            return;
            
        // Get note name to determine if it's a sharp
        juce::String noteName = button.getButtonText();
        bool isSharp = Theme::isSharpNote(noteName);
        bool isPressed = button.getToggleState() || isButtonDown;
        
        // Draw key tile with proper gradient and shadows
        // Pulse effect will be handled by the pressed state for simplicity
        drawKeyTile(g, bounds, isSharp, isPressed, isMouseOverButton);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                         bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        
        // Ensure we have valid dimensions
        if (bounds.getWidth() <= 0 || bounds.getHeight() <= 0)
            return;
            
        juce::String noteName = button.getButtonText();
        
        // Set up text styling with proper weight and spacing
        auto font = Theme::keyLabel();
        font.setHorizontalScale(0.98f); // Slightly condensed for better fit
        g.setFont (font);
        
        // Use black text when button is toggled on (selected) for better contrast against orange gradient
        if (button.getToggleState()) {
            g.setColour (juce::Colours::black);
        } else {
            g.setColour (Theme::textPrimary);
        }
        
        // Draw normal text - no special glyph tinting
        g.drawFittedText (noteName, bounds.toNearestInt(), juce::Justification::centred, 1);
    }

    // ============================================================================
    // KEY TILE DRAWING SYSTEM
    // ============================================================================
    
    // Animation support for smooth state transitions with micro-interactions
    void animateKeyTilePress(juce::Button& button, bool isPressed)
    {
        // No scale animation - button stays in place when pressed
        // Trigger press pulse animation for visual feedback
        if (isPressed) {
            triggerPressPulse(button);
        }
        
        // Note: Haptic feedback will be handled by the MainComponent
        // to avoid Objective-C++ compilation issues in the header
    }
    
    // Key press pulse animation - 6-8px outer glow that blooms and fades
    void triggerPressPulse(juce::Button& button)
    {
        // Use a simple timer-based approach for pulse animation
        // The pulse will be handled by the animation system
        button.repaint();
        
        // Auto-reset pulse after 200ms
        juce::Timer::callAfterDelay(200, [&button]() {
            button.repaint();
        });
    }
    
private:
    void drawKeyTile (juce::Graphics& g, juce::Rectangle<float> bounds, 
                     bool isSharp, bool isPressed, bool isHovered)
    {
        // Apply corner radius
        float cornerRadius = Theme::keyCornerRadius;
        
        // Draw shadows first (layered shadow system)
        drawKeyTileShadows(g, bounds, isPressed, isHovered);
        
        // Draw main key tile with gradient
        drawKeyTileGradient(g, bounds, isSharp, isPressed, cornerRadius);
        
        // Draw border - DISABLED FOR DEBUG
        // drawKeyTileBorder(g, bounds, cornerRadius);
        
        // Draw inner highlights and shades - DISABLED FOR DEBUG
        // drawKeyTileInnerEffects(g, bounds, cornerRadius);
        
        // Draw pressed state effects according to spec
        if (isPressed) {
            // 3.1 Micro-specular line at top (inset)
            drawMicroSpecularLine(g, bounds, cornerRadius);
            
            // 3.2 Accent flash (inner glow pop) - orange center fill only
            drawAccentFlash(g, bounds, cornerRadius, isSharp);
            
            // Note: Text color is now handled directly in drawButtonText() for better contrast
        }
        
        // Draw focus ring for hover/focus state (non-pressed) - DISABLED FOR DEBUG
        // if (isHovered && !isPressed) {
        //     drawKeyTileFocusRing(g, bounds, cornerRadius);
        // }
    }
    
    void drawKeyTileShadows (juce::Graphics& g, juce::Rectangle<float> bounds, 
                           bool isPressed, bool isHovered)
    {
        // 3.4 Shadow system according to spec
        if (isPressed) {
            // Pressed: reduced outer shadow for compression effect
            g.setColour (Theme::shadowPressedOuter);
            auto shadowBounds = bounds.expanded(9.0f); // 18px blur / 2
            shadowBounds = shadowBounds.translated(0, 6.0f); // 6px offset
            g.drawRoundedRectangle (shadowBounds, Theme::keyCornerRadius, 0.0f);
            
            // Inner shadows for depth
            g.setColour (Theme::shadowPressedInnerTop);
            g.drawRoundedRectangle (bounds.expanded(1.0f), Theme::keyCornerRadius, 1.0f);
            
            g.setColour (Theme::shadowPressedInnerBottom);
            g.drawRoundedRectangle (bounds.expanded(1.0f), Theme::keyCornerRadius, 1.5f);
        } else {
            // Rest state: standard shadow
            juce::Colour shadowColor = isHovered ? Theme::shadowOuterHover : Theme::shadowOuter;
            float shadowBlur = isHovered ? 28.0f : 24.0f;
            float shadowOffset = 8.0f;
            
            g.setColour (shadowColor);
            auto shadowBounds = bounds.expanded(shadowBlur * 0.5f);
            shadowBounds = shadowBounds.translated(0, shadowOffset);
            g.drawRoundedRectangle (shadowBounds, Theme::keyCornerRadius, 0.0f);
            
            // Edge crispness shadow for definition
            g.setColour (Theme::shadowEdge);
            g.drawRoundedRectangle (bounds.expanded(1.0f), Theme::keyCornerRadius, 1.0f);
        }
    }
    
    void drawKeyTileGradient (juce::Graphics& g, juce::Rectangle<float> bounds, 
                            bool isSharp, bool isPressed, float cornerRadius)
    {
        // Get appropriate colors for the key type and state
        juce::Colour topColor = Theme::getKeyTopColor(isSharp, isPressed);
        juce::Colour bottomColor = Theme::getKeyBottomColor(isSharp, isPressed);
        
        // Create vertical gradient
        juce::ColourGradient gradient (topColor, bounds.getX(), bounds.getY(),
                                      bottomColor, bounds.getX(), bounds.getBottom(),
                                      false);
        
        g.setGradientFill (gradient);
        g.fillRoundedRectangle (bounds, cornerRadius);
    }
    
    void drawKeyTileBorder (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius)
    {
        g.setColour (Theme::keyBorder);
        g.drawRoundedRectangle (bounds.reduced(0.5f), cornerRadius, Theme::keyBorderWidth);
    }
    
    void drawKeyTileInnerEffects (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius)
    {
        // Inner top highlight (15-20% from top)
        auto topHighlightBounds = bounds.removeFromTop(bounds.getHeight() * 0.18f);
        topHighlightBounds = topHighlightBounds.expanded(0, -cornerRadius * 0.5f);
        
        if (topHighlightBounds.getHeight() > 0) {
            g.setColour (Theme::keyInnerTopHighlight);
            g.fillRoundedRectangle (topHighlightBounds, cornerRadius);
        }
        
        // Inner bottom shade (15-20% from bottom)
        auto bottomShadeBounds = bounds.removeFromBottom(bounds.getHeight() * 0.18f);
        bottomShadeBounds = bottomShadeBounds.expanded(0, -cornerRadius * 0.5f);
        
        if (bottomShadeBounds.getHeight() > 0) {
            g.setColour (Theme::keyInnerBottomShade);
            g.fillRoundedRectangle (bottomShadeBounds, cornerRadius);
        }
    }
    
    void drawKeyTileFocusRing (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius)
    {
        // Focus ring with accent glow - subtle outer glow
        auto focusBounds = bounds.expanded(4.0f);
        
        // Outer glow effect removed (blue outline removed)
        
        // Inner focus ring removed (blue outline removed)
    }
    
    void drawSharpNoteEdgeGlow (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius)
    {
        // Subtle cool edge glow for sharp notes when pressed
        auto glowBounds = bounds.expanded(3.0f);
        
        // Outer cool glow removed (blue outline removed)
        
        // Inner cool accent removed (blue outline removed)
    }
    
    // 3.1 Micro-specular line at top (inset) - white @ 8% opacity over top 14-18%
    void drawMicroSpecularLine (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius)
    {
        auto specularBounds = bounds.removeFromTop(bounds.getHeight() * 0.16f); // 16% of height
        specularBounds = specularBounds.expanded(-2.0f, -cornerRadius * 0.3f);
        
        if (specularBounds.getHeight() > 0) {
            g.setColour (juce::Colour (0x14FFFFFF)); // White @ 8% opacity
            g.fillRoundedRectangle (specularBounds, cornerRadius * 0.8f);
        }
    }
    
    // 3.2 Accent flash (inner glow pop) - subtle orange gradient for all keys when pressed
    void drawAccentFlash (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius, bool isSharp)
    {
        auto center = bounds.getCentre();
        auto maxRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.25f; // 25% of tile (smaller coverage)
        
        // Subtle gradient using 10 circles with better fade
        for (int i = 0; i < 10; ++i)
        {
            float progress = i / 9.0f; // 0.0 to 1.0
            float radius = maxRadius * (1.0f - progress * 0.6f); // From 100% to 40% radius
            float alpha = (1.0f - progress) * (1.0f - progress) * 0.6f; // Quadratic fade from 60% to 0% opacity
            
            g.setColour (juce::Colour::fromRGBA(255, 165, 0, (int)(alpha * 255)));
            g.fillEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        }
    }
    
    // 3.3 Focus ring (outer ring under shadow) - clean orange for all keys when pressed
    void drawFocusRing (juce::Graphics& g, juce::Rectangle<float> bounds, float cornerRadius, bool isSharp)
    {
        // Clean orange ring - no blue elements
        g.setColour (juce::Colours::orange);
        auto ringBounds = bounds.expanded(6.0f);
        g.drawRoundedRectangle (ringBounds, cornerRadius + 6.0f, 2.0f);
    }
    
    // 3.5 Text brightening overlay - white overlay @ 10% for +3-5% luminance
    void drawTextBrightening (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour (Theme::textBrightenOverlay);
        g.fillRoundedRectangle (bounds, Theme::keyCornerRadius);
    }
};

// ============================================================================
// BACKGROUND GRADIENT SYSTEM
// ============================================================================

struct BackgroundGradient
{
    static void drawCanvasBackground (juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        auto floatBounds = bounds.toFloat();
        
        // Main vertical gradient from top to bottom
        juce::ColourGradient mainGradient (Theme::bgCanvasTop, 
                                          floatBounds.getX(), floatBounds.getY(),
                                          Theme::bgCanvasBottom, 
                                          floatBounds.getX(), floatBounds.getBottom(),
                                          false);
        
        g.setGradientFill (mainGradient);
        g.fillRect (bounds);
        
        // Optional subtle radial vignette
        drawRadialVignette(g, floatBounds);
    }
    
private:
    static void drawRadialVignette (juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        auto center = bounds.getCentre();
        auto radius = juce::jmax(bounds.getWidth(), bounds.getHeight()) * 0.7f;
        
        // Radial vignette: center #000000 at 6-8% opacity to edges
        juce::ColourGradient vignette (juce::Colour (0x10000000), center.x, center.y,
                                      juce::Colour (0x00000000), center.x, center.y,
                                      true);
        vignette.addColour(0.8f, juce::Colour (0x08000000));
        
        g.setGradientFill (vignette);
        g.fillEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    }
    
    #pragma GCC diagnostic pop
};
