#include "MetallicLookAndFeel.h"

// ============================================================================
// COLOR DEFINITIONS (Exact web match)
// ============================================================================

// Step sequencer effect colors
const juce::Colour MetallicLookAndFeel::FILTER_COLOR   = juce::Colour(0xffff6b35);  // Orange
const juce::Colour MetallicLookAndFeel::PAN_COLOR      = juce::Colour(0xff00d4ff);  // Cyan
const juce::Colour MetallicLookAndFeel::DELAY_COLOR    = juce::Colour(0xff00ff88);  // Green
const juce::Colour MetallicLookAndFeel::REVERB_COLOR   = juce::Colour(0xffff00ff);  // Magenta
const juce::Colour MetallicLookAndFeel::VOLUME_COLOR   = juce::Colour(0xffffaa00);  // Amber

// Metallic panel colors
const juce::Colour MetallicLookAndFeel::METAL_DARK      = juce::Colour(0xff4a4a4d);
const juce::Colour MetallicLookAndFeel::METAL_MID_DARK  = juce::Colour(0xff5a5a5f);
const juce::Colour MetallicLookAndFeel::METAL_MID       = juce::Colour(0xff6a6a70);
const juce::Colour MetallicLookAndFeel::METAL_MID_LIGHT = juce::Colour(0xff8a8a8f);

// Recessed/dark areas
const juce::Colour MetallicLookAndFeel::RECESS_DARK   = juce::Colour(0xff1a1a1c);
const juce::Colour MetallicLookAndFeel::RECESS_MID    = juce::Colour(0xff2a2a2d);
const juce::Colour MetallicLookAndFeel::RECESS_LIGHT  = juce::Colour(0xff3a3a3d);

// Border colors
const juce::Colour MetallicLookAndFeel::BORDER_DARK = juce::Colour(0xff0a0a0d);
const juce::Colour MetallicLookAndFeel::BORDER_MID  = juce::Colour(0xff555558);

// Text colors
const juce::Colour MetallicLookAndFeel::TEXT_PRIMARY   = juce::Colour(0xffe8e8ea);
const juce::Colour MetallicLookAndFeel::TEXT_SECONDARY = juce::Colour(0xffd0d0d3);
const juce::Colour MetallicLookAndFeel::TEXT_TERTIARY  = juce::Colour(0xff9a9a9d);
const juce::Colour MetallicLookAndFeel::TEXT_DISABLED  = juce::Colour(0xff7a7a7d);
const juce::Colour MetallicLookAndFeel::TEXT_DARK      = juce::Colour(0xff5a5a5d);

// Accent colors
const juce::Colour MetallicLookAndFeel::ACCENT_BRIGHT  = juce::Colour(0xff8ed4ff);
const juce::Colour MetallicLookAndFeel::ACCENT_MID     = juce::Colour(0xff78c4e8);
const juce::Colour MetallicLookAndFeel::ACCENT_MEDIUM  = juce::Colour(0xff5aa8cc);
const juce::Colour MetallicLookAndFeel::ACCENT_DARK    = juce::Colour(0xff3d8db0);

// ============================================================================
// CONSTRUCTOR
// ============================================================================

MetallicLookAndFeel::MetallicLookAndFeel()
{
    // Set default colors
    setColour(juce::TextButton::textColourOffId, TEXT_TERTIARY);
    setColour(juce::TextButton::textColourOnId, TEXT_PRIMARY);
    setColour(juce::Label::textColourId, TEXT_SECONDARY);
}

// ============================================================================
// EFFECT COLOR GETTER
// ============================================================================

juce::Colour MetallicLookAndFeel::getEffectColour(EffectType type)
{
    switch (type)
    {
        case EffectType::Filter:  return FILTER_COLOR;
        case EffectType::Pan:     return PAN_COLOR;
        case EffectType::Delay:   return DELAY_COLOR;
        case EffectType::Reverb:  return REVERB_COLOR;
        case EffectType::Volume:  return VOLUME_COLOR;
        default:                  return FILTER_COLOR;
    }
}

// ============================================================================
// PANEL RENDERING
// ============================================================================

void MetallicLookAndFeel::drawMainPanel(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // CSS: linear-gradient(135deg, #4a4a4d 0%, #5a5a5f 25%, #6a6a70 50%, #5a5a5f 75%, #4a4a4d 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(METAL_DARK, floatBounds.getTopLeft(),
                                   METAL_MID, floatBounds.getBottomRight(), false);
    gradient.addColour(0.25, METAL_MID_DARK);
    gradient.addColour(0.50, METAL_MID);
    gradient.addColour(0.75, METAL_MID_DARK);
    gradient.addColour(1.00, METAL_DARK);
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, PANEL_RADIUS);
    
    // Border
    g.setColour(RECESS_LIGHT);
    g.drawRoundedRectangle(floatBounds, PANEL_RADIUS, 1.0f);
    
    // Inner highlight: inset 0 1px 0 rgba(255, 255, 255, 0.15)
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    g.drawLine(floatBounds.getX() + PANEL_RADIUS, floatBounds.getY() + 1.0f,
               floatBounds.getRight() - PANEL_RADIUS, floatBounds.getY() + 1.0f, 1.0f);
    
    // Inner shadow: inset 0 -1px 0 rgba(0, 0, 0, 0.5)
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawLine(floatBounds.getX() + PANEL_RADIUS, floatBounds.getBottom() - 1.0f,
               floatBounds.getRight() - PANEL_RADIUS, floatBounds.getBottom() - 1.0f, 1.0f);
}

void MetallicLookAndFeel::drawHeader(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // CSS: linear-gradient(180deg, #5a5a5f 0%, #4a4a4d 50%, #3a3a3d 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(METAL_MID_DARK, floatBounds.getTopLeft(),
                                   RECESS_LIGHT, floatBounds.getBottomLeft(), false);
    gradient.addColour(0.50, METAL_DARK);
    
    g.setGradientFill(gradient);
    g.fillRect(floatBounds);
    
    // Inner highlight
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawLine(floatBounds.getX(), floatBounds.getY(),
               floatBounds.getRight(), floatBounds.getY(), 1.0f);
    
    // Inner shadow
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.drawLine(floatBounds.getX(), floatBounds.getBottom(),
               floatBounds.getRight(), floatBounds.getBottom(), 1.0f);
}

void MetallicLookAndFeel::drawRecessedPanel(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // CSS: linear-gradient(135deg, #2a2a2d 0%, #3a3a3d 50%, #2a2a2d 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(RECESS_MID, floatBounds.getTopLeft(),
                                   RECESS_LIGHT, floatBounds.getBottomRight(), false);
    gradient.addColour(0.50, RECESS_LIGHT);
    gradient.addColour(1.00, RECESS_MID);
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, SECTION_RADIUS);
    
    // Border
    g.setColour(RECESS_DARK);
    g.drawRoundedRectangle(floatBounds, SECTION_RADIUS, 1.0f);
    
    // Inner shadow: inset 0 2px 4px rgba(0, 0, 0, 0.6)
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.drawRoundedRectangle(floatBounds.reduced(1.0f), SECTION_RADIUS - 1.0f, 1.0f);
    
    // Highlight: inset 0 -1px 1px rgba(255, 255, 255, 0.05)
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawLine(floatBounds.getX() + SECTION_RADIUS, floatBounds.getBottom() - 1.0f,
               floatBounds.getRight() - SECTION_RADIUS, floatBounds.getBottom() - 1.0f, 1.0f);
}

void MetallicLookAndFeel::drawBadge(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // CSS: linear-gradient(135deg, #6a6a6d 0%, #8a8a8f 50%, #6a6a6d 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(juce::Colour(0xff6a6a6d), floatBounds.getTopLeft(),
                                   juce::Colour(0xff6a6a6d), floatBounds.getBottomRight(), false);
    gradient.addColour(0.50, METAL_MID_LIGHT);
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, 4.0f);
    
    // Border
    g.setColour(METAL_DARK);
    g.drawRoundedRectangle(floatBounds, 4.0f, 1.0f);
    
    // Inner effects
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawLine(floatBounds.getX(), floatBounds.getY() + 1.0f,
               floatBounds.getRight(), floatBounds.getY() + 1.0f, 1.0f);
    
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawLine(floatBounds.getX(), floatBounds.getBottom() - 1.0f,
               floatBounds.getRight(), floatBounds.getBottom() - 1.0f, 1.0f);
}

// ============================================================================
// NEW BEVELED HARDWARE-STYLE PANELS
// ============================================================================

void MetallicLookAndFeel::drawOuterChassis(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Outer metal chassis with raised bezel
    // background: linear-gradient(145deg, #4a5568 0%, #2d3748 50%, #1a202c 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(juce::Colour(0xff4a5568), floatBounds.getTopLeft(),
                                   juce::Colour(0xff1a202c), floatBounds.getBottomRight(), false);
    gradient.addColour(0.50, juce::Colour(0xff2d3748));
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, PANEL_RADIUS);
    
    // Multiple shadow layers for depth
    // boxShadow: inset -3px -3px 8px rgba(0,0,0,0.6), inset 3px 3px 8px rgba(255,255,255,0.1)
    juce::Path innerShadow;
    innerShadow.addRoundedRectangle(floatBounds.reduced(2.0f), PANEL_RADIUS - 2.0f);
    
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.strokePath(innerShadow, juce::PathStrokeType(1.5f));
    
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle(floatBounds.reduced(1.0f), PANEL_RADIUS - 1.0f, 1.0f);
}

void MetallicLookAndFeel::drawInnerRecessedPanel(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Inner recessed panel
    // background: linear-gradient(180deg, #1a1f2e 0%, #252d3d 50%, #1a1f2e 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(juce::Colour(0xff1a1f2e), floatBounds.getTopLeft(),
                                   juce::Colour(0xff1a1f2e), floatBounds.getBottomLeft(), false);
    gradient.addColour(0.50, juce::Colour(0xff252d3d));
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, SECTION_RADIUS);
    
    // Deep inset shadow
    // boxShadow: inset 0 4px 12px rgba(0,0,0,0.8), inset 0 -2px 8px rgba(255,255,255,0.03)
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawRoundedRectangle(floatBounds.reduced(1.0f), SECTION_RADIUS - 1.0f, 2.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.03f));
    g.drawLine(floatBounds.getX() + SECTION_RADIUS, floatBounds.getBottom() - 2.0f,
               floatBounds.getRight() - SECTION_RADIUS, floatBounds.getBottom() - 2.0f, 1.0f);
    
    // Border
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRoundedRectangle(floatBounds, SECTION_RADIUS, 1.0f);
}

void MetallicLookAndFeel::drawDeeplyRecessedSection(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Deeply recessed control section
    // background: linear-gradient(180deg, #0f1419 0%, #1a1f2e 100%)
    auto floatBounds = bounds.toFloat();
    
    juce::ColourGradient gradient(juce::Colour(0xff0f1419), floatBounds.getTopLeft(),
                                   juce::Colour(0xff1a1f2e), floatBounds.getBottomLeft(), false);
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, 8.0f);
    
    // Very deep shadow
    // boxShadow: inset 0 3px 8px rgba(0,0,0,0.7), inset 0 -1px 3px rgba(255,255,255,0.02)
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.drawRoundedRectangle(floatBounds.reduced(1.0f), 7.0f, 2.5f);
    
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    g.drawLine(floatBounds.getX() + 8.0f, floatBounds.getBottom() - 1.0f,
               floatBounds.getRight() - 8.0f, floatBounds.getBottom() - 1.0f, 1.0f);
    
    // Dark border
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRoundedRectangle(floatBounds, 8.0f, 1.0f);
    
    // Subtle top highlight
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawLine(floatBounds.getX() + 8.0f, floatBounds.getY() + 1.0f,
               floatBounds.getRight() - 8.0f, floatBounds.getY() + 1.0f, 1.0f);
}

void MetallicLookAndFeel::drawCornerScrew(juce::Graphics& g, juce::Point<float> center, float radius)
{
    // Rack-mount screw with realistic depth
    // Radial gradient from light to dark
    juce::ColourGradient screwGradient(juce::Colour(0xff718096), center.x - radius * 0.3f, center.y - radius * 0.3f,
                                        juce::Colour(0xff2d3748), center.x + radius * 0.7f, center.y + radius * 0.7f, true);
    screwGradient.addColour(0.60, juce::Colour(0xff3a4558));
    
    g.setGradientFill(screwGradient);
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    
    // Inset shadow
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(center.x - radius + 1.0f, center.y - radius + 1.0f, 
                  radius * 2.0f - 2.0f, radius * 2.0f - 2.0f, 1.0f);
    
    // Top highlight
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawEllipse(center.x - radius, center.y - radius, 
                  radius * 2.0f, radius * 2.0f, 1.0f);
}

void MetallicLookAndFeel::drawEmbossedBadge(juce::Graphics& g, juce::Rectangle<int> bounds,
                                             const juce::String& lightText, const juce::String& boldText)
{
    // Embossed badge background
    // background: linear-gradient(145deg, #374151 0%, #1f2937 100%)
    auto floatBounds = bounds.toFloat();
    
    // Debug: ensure we have valid bounds
    if (floatBounds.getWidth() < 10.0f || floatBounds.getHeight() < 10.0f)
        return;
    
    juce::ColourGradient gradient(juce::Colour(0xff374151), floatBounds.getTopLeft(),
                                   juce::Colour(0xff1f2937), floatBounds.getBottomRight(), false);
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(floatBounds, 4.0f);
    
    // Inset shadow
    // boxShadow: inset 2px 2px 4px rgba(0,0,0,0.4), inset -1px -1px 3px rgba(255,255,255,0.05)
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.drawRoundedRectangle(floatBounds.reduced(1.5f), 3.0f, 1.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawRoundedRectangle(floatBounds.reduced(0.5f), 3.5f, 0.5f);
    
    // Border
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRoundedRectangle(floatBounds, 4.0f, 1.0f);
    
    // Draw "LATHAM" in LIGHT font and "AUDIO" in BOLD font separately
    auto textBounds = floatBounds.reduced(2.0f); // Minimal reduction for maximum text space
    
    // Using deprecated Font constructor until JUCE 8 API is stable
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #endif
    
    juce::Font lightFont(16.0f, juce::Font::plain); // Light/regular weight
    juce::Font boldFont(16.0f, juce::Font::bold);   // Bold weight
    
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
    
    // Measure text widths to position them (using GlyphArrangement to avoid deprecation)
    juce::GlyphArrangement lathamGlyphs, audioGlyphs;
    lathamGlyphs.addLineOfText(lightFont, lightText, 0.0f, 0.0f);
    audioGlyphs.addLineOfText(boldFont, boldText, 0.0f, 0.0f);
    
    float lathamWidth = lathamGlyphs.getBoundingBox(0, -1, true).getWidth();
    float audioWidth = audioGlyphs.getBoundingBox(0, -1, true).getWidth();
    float totalWidth = lathamWidth + audioWidth + 4.0f; // 4px spacing between words
    
    // Center the combined text
    float startX = textBounds.getX() + (textBounds.getWidth() - totalWidth) * 0.5f;
    float textY = textBounds.getCentreY();
    
    // Draw "LATHAM" shadow
    g.setFont(lightFont);
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawText(lightText, startX, textY - 8.0f + 1.0f, lathamWidth + 10.0f, 16.0f, 
               juce::Justification::left, false);
    
    // Draw "LATHAM" text (light)
    g.setColour(juce::Colour(0xffeeeeee));
    g.drawText(lightText, startX, textY - 8.0f, lathamWidth + 10.0f, 16.0f, 
               juce::Justification::left, false);
    
    // Draw "AUDIO" shadow
    g.setFont(boldFont);
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawText(boldText, startX + lathamWidth + 4.0f, textY - 8.0f + 1.0f, audioWidth + 10.0f, 16.0f, 
               juce::Justification::left, false);
    
    // Draw "AUDIO" text (bold)
    g.setColour(juce::Colour(0xffeeeeee));
    g.drawText(boldText, startX + lathamWidth + 4.0f, textY - 8.0f, audioWidth + 10.0f, 16.0f, 
               juce::Justification::left, false);
}

// ============================================================================
// BUTTON RENDERING
// ============================================================================

void MetallicLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                juce::Button& button,
                                                const juce::Colour& backgroundColour,
                                                bool isMouseOverButton,
                                                bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    
    // Check if this is an accent button (based on component ID or property)
    bool isAccent = button.getName().contains("accent") || 
                    button.getName().contains("randomize");
    
    if (isAccent)
        drawAccentButton(g, bounds, isMouseOverButton, isButtonDown);
    else
        drawNormalButton(g, bounds, isMouseOverButton, isButtonDown);
}

void MetallicLookAndFeel::drawNormalButton(juce::Graphics& g, juce::Rectangle<float> bounds,
                                           bool isOver, bool isDown)
{
    juce::ColourGradient gradient;
    
    if (isDown)
    {
        // Active: linear-gradient(180deg, #3a3a3d 0%, #4a4a4d 100%)
        gradient = juce::ColourGradient(RECESS_LIGHT, bounds.getTopLeft(),
                                        METAL_DARK, bounds.getBottomLeft(), false);
        
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Inner shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), BUTTON_RADIUS - 1.0f, 1.0f);
    }
    else if (isOver)
    {
        // Hover: linear-gradient(180deg, #6a6a6f 0%, #5a5a5f 100%)
        gradient = juce::ColourGradient(juce::Colour(0xff6a6a6f), bounds.getTopLeft(),
                                        METAL_MID_DARK, bounds.getBottomLeft(), false);
        
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawLine(bounds.getX(), bounds.getY() + 1.0f,
                   bounds.getRight(), bounds.getY() + 1.0f, 1.0f);
    }
    else
    {
        // Normal: linear-gradient(180deg, #5a5a5f 0%, #4a4a4d 100%)
        gradient = juce::ColourGradient(METAL_MID_DARK, bounds.getTopLeft(),
                                        METAL_DARK, bounds.getBottomLeft(), false);
        
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawLine(bounds.getX(), bounds.getY() + 1.0f,
                   bounds.getRight(), bounds.getY() + 1.0f, 1.0f);
        
        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawLine(bounds.getX(), bounds.getBottom() - 1.0f,
                   bounds.getRight(), bounds.getBottom() - 1.0f, 1.0f);
    }
    
    // Border
    g.setColour(BORDER_MID);
    g.drawRoundedRectangle(bounds, BUTTON_RADIUS, 1.0f);
}

void MetallicLookAndFeel::drawAccentButton(juce::Graphics& g, juce::Rectangle<float> bounds,
                                           bool isOver, bool isDown)
{
    juce::ColourGradient gradient;
    
    if (isDown)
    {
        // Active: linear-gradient(135deg, #3d8db0 0%, #2d7d9a 50%, #1d6d8a 100%)
        gradient = juce::ColourGradient(ACCENT_DARK, bounds.getTopLeft(),
                                        juce::Colour(0xff1d6d8a), bounds.getBottomRight(), false);
        gradient.addColour(0.50, juce::Colour(0xff2d7d9a));
        
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Inner shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), BUTTON_RADIUS - 1.0f, 1.0f);
    }
    else if (isOver)
    {
        // Hover: linear-gradient(135deg, #8ed4ff 0%, #6ab8dc 50%, #4d9dc0 100%)
        gradient = juce::ColourGradient(ACCENT_BRIGHT, bounds.getTopLeft(),
                                        juce::Colour(0xff4d9dc0), bounds.getBottomRight(), false);
        gradient.addColour(0.50, juce::Colour(0xff6ab8dc));
        
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Glow effect
        g.setColour(ACCENT_DARK.withAlpha(0.8f));
        g.fillRoundedRectangle(bounds.expanded(2.0f), BUTTON_RADIUS + 2.0f);
        
        // Refill main button
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawLine(bounds.getX(), bounds.getY() + 1.0f,
                   bounds.getRight(), bounds.getY() + 1.0f, 1.0f);
    }
    else
    {
        // Normal: linear-gradient(135deg, #78c4e8 0%, #5aa8cc 50%, #3d8db0 100%)
        gradient = juce::ColourGradient(ACCENT_MID, bounds.getTopLeft(),
                                        ACCENT_DARK, bounds.getBottomRight(), false);
        gradient.addColour(0.50, ACCENT_MEDIUM);
        
        // Glow first
        g.setColour(ACCENT_DARK.withAlpha(0.6f));
        g.fillRoundedRectangle(bounds.expanded(3.0f), BUTTON_RADIUS + 3.0f);
        
        // Main button
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, BUTTON_RADIUS);
        
        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.drawLine(bounds.getX(), bounds.getY() + 1.0f,
                   bounds.getRight(), bounds.getY() + 1.0f, 1.0f);
        
        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.drawLine(bounds.getX(), bounds.getBottom() - 1.0f,
                   bounds.getRight(), bounds.getBottom() - 1.0f, 1.0f);
    }
}

// ============================================================================
// KNOB (ROTARY SLIDER) RENDERING - 3D BEVELED HARDWARE STYLE
// ============================================================================

void MetallicLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPosProportional, float rotaryStartAngle,
                                            float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(x, y, width, height);
    auto center = bounds.getCentre();
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    
    // Outer beveled bezel - raised effect
    juce::ColourGradient outerBezel(juce::Colour(0xff5a6578), center.x - radius, center.y - radius,
                                     juce::Colour(0xff1a2538), center.x + radius, center.y + radius, false);
    outerBezel.addColour(0.30, juce::Colour(0xff3a4558));
    outerBezel.addColour(0.70, juce::Colour(0xff2a3548));
    
    g.setGradientFill(outerBezel);
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    
    // Inner recessed area (dark)
    float recessRadius = radius * 0.92f;
    juce::ColourGradient recessGrad(juce::Colour(0xff14191f), center.x, center.y,
                                     juce::Colour(0xff0a0f15), center.x + recessRadius, center.y + recessRadius, true);
    g.setGradientFill(recessGrad);
    g.fillEllipse(center.x - recessRadius, center.y - recessRadius, recessRadius * 2.0f, recessRadius * 2.0f);
    
    // Track groove (dark ring)
    float trackRadius = radius * 0.79f;
    float trackWidth = radius * 0.125f;
    
    juce::Path trackArc;
    trackArc.addCentredArc(center.x, center.y, trackRadius, trackRadius,
                          0.0f, rotaryStartAngle, rotaryEndAngle, true);
    
    g.setColour(juce::Colour(0xff0a0a0c).withAlpha(0.8f));
    g.strokePath(trackArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved));
    
    // Progress arc with glow
    if (sliderPosProportional > 0.001f)
    {
        juce::Path progressArc;
        progressArc.addCentredArc(center.x, center.y, trackRadius, trackRadius,
                                 0.0f, rotaryStartAngle, angle, true);
        
        // Glow effect
        juce::Colour glowColor = juce::Colour(0xff3b82f6); // Blue
        g.setColour(glowColor.withAlpha(0.8f));
        g.strokePath(progressArc, juce::PathStrokeType(trackWidth + 2.0f, juce::PathStrokeType::curved));
        
        // Main progress arc
        juce::ColourGradient progressGrad(juce::Colour(0xff60a5fa), center.x - trackRadius, center.y - trackRadius,
                                          juce::Colour(0xff2563eb), center.x + trackRadius, center.y + trackRadius, false);
        progressGrad.addColour(0.50, juce::Colour(0xff3b82f6));
        
        g.setGradientFill(progressGrad);
        g.strokePath(progressArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved));
    }
    
    // Center knob body - beveled metallic
    float knobRadius = radius * 0.58f;
    
    // Outer knob gradient
    juce::ColourGradient knobOuter(juce::Colour(0xff4a5568), center.x - knobRadius, center.y - knobRadius,
                                   juce::Colour(0xff1a202c), center.x + knobRadius, center.y + knobRadius, false);
    knobOuter.addColour(0.50, juce::Colour(0xff2d3748));
    
    g.setGradientFill(knobOuter);
    g.fillEllipse(center.x - knobRadius, center.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
    
    // Inner knob with radial gradient
    float innerKnobRadius = knobRadius * 0.93f;
    juce::ColourGradient knobInner(juce::Colour(0xff3a4556), center.x, center.y,
                                   juce::Colour(0xff1a2536), center.x + innerKnobRadius, center.y + innerKnobRadius, true);
    knobInner.addColour(0.60, juce::Colour(0xff2a3546));
    
    g.setGradientFill(knobInner);
    g.fillEllipse(center.x - innerKnobRadius, center.y - innerKnobRadius, 
                  innerKnobRadius * 2.0f, innerKnobRadius * 2.0f);
    
    // Knob highlight (top oval for 3D effect)
    float highlightRadius = knobRadius * 0.4f;
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.fillEllipse(center.x - highlightRadius, center.y - knobRadius * 0.5f,
                  highlightRadius * 2.0f, highlightRadius * 1.3f);
    
    // Indicator line with metallic look
    juce::Path indicator;
    float indicatorLength = knobRadius * 0.85f;
    float indicatorY = center.y - indicatorLength;
    
    indicator.startNewSubPath(center.x, center.y);
    indicator.lineTo(center.x, indicatorY);
    
    juce::AffineTransform rotation = juce::AffineTransform::rotation(angle, center.x, center.y);
    indicator.applyTransform(rotation);
    
    // Indicator gradient (metallic)
    juce::ColourGradient indicatorGrad(juce::Colour(0xfff0f0f0), center.x, center.y,
                                       juce::Colour(0xffa0a0a0), center.x, indicatorY, false);
    indicatorGrad.addColour(0.50, juce::Colour(0xffd0d0d0));
    
    g.setGradientFill(indicatorGrad);
    g.strokePath(indicator, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Center cap screw
    float capRadius = radius * 0.08f;
    juce::ColourGradient capGrad(juce::Colour(0xff6a7588), center.x - capRadius * 0.3f, center.y - capRadius * 0.3f,
                                 juce::Colour(0xff1a2538), center.x + capRadius * 0.7f, center.y + capRadius * 0.7f, true);
    capGrad.addColour(0.60, juce::Colour(0xff3a4558));
    
    g.setGradientFill(capGrad);
    g.fillEllipse(center.x - capRadius, center.y - capRadius, capRadius * 2.0f, capRadius * 2.0f);
}

// ============================================================================
// STEP SEQUENCER RENDERING
// ============================================================================

void MetallicLookAndFeel::drawStepBarBackground(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Deeply recessed step slot with beveled edges
    // background: linear-gradient(145deg, #1a1f2e 0%, #0f1419 100%)
    juce::ColourGradient bg(juce::Colour(0xff1a1f2e), bounds.getTopLeft(),
                            juce::Colour(0xff0f1419), bounds.getBottomRight(), false);
    
    g.setGradientFill(bg);
    g.fillRoundedRectangle(bounds, 3.0f);
    
    // Multiple shadow layers for depth
    // boxShadow: inset 0 3px 6px rgba(0,0,0,0.8), inset 0 -1px 2px rgba(255,255,255,0.03)
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 2.0f, 2.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.03f));
    g.drawLine(bounds.getX() + 3.0f, bounds.getBottom() - 1.0f,
               bounds.getRight() - 3.0f, bounds.getBottom() - 1.0f, 1.0f);
    
    // Side shadows for beveled inset
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawLine(bounds.getX() + 1.0f, bounds.getY() + 3.0f,
               bounds.getX() + 1.0f, bounds.getBottom() - 3.0f, 1.0f);
    g.drawLine(bounds.getRight() - 1.0f, bounds.getY() + 3.0f,
               bounds.getRight() - 1.0f, bounds.getBottom() - 3.0f, 1.0f);
    
    // Dark border
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
}

void MetallicLookAndFeel::drawStepBarFill(juce::Graphics& g,
                                          juce::Rectangle<float> bounds,
                                          juce::Colour effectColor,
                                          float value,
                                          bool isActive)
{
    float height = bounds.getHeight() * value;
    auto fillBounds = bounds.withTop(bounds.getBottom() - height);
    
    if (fillBounds.getHeight() < 1.0f)
        return;
    
    if (isActive)
    {
        // Active: with beveled top and glow
        // linear-gradient(180deg, ${color}ff 0%, ${color}ee 50%, ${color}cc 100%)
        
        // Outer glow: 0 0 16px ${color}cc
        g.setColour(effectColor.withAlpha(0.8f));
        g.fillRoundedRectangle(fillBounds.expanded(2.0f), 4.0f);
        
        // Secondary glow layer
        g.setColour(effectColor.withAlpha(0.55f));
        g.fillRoundedRectangle(fillBounds.expanded(4.0f), 5.0f);
        
        // Gradient fill with beveled effect
        juce::ColourGradient fill(effectColor, fillBounds.getTopLeft(),
                                  effectColor.withAlpha(0.8f), fillBounds.getBottomLeft(), false);
        fill.addColour(0.50, effectColor.withAlpha(0.93f));
        g.setGradientFill(fill);
        g.fillRoundedRectangle(fillBounds, STEP_RADIUS);
        
        // Inner highlight: inset 0 1px 2px rgba(255, 255, 255, 0.3)
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawLine(fillBounds.getX(), fillBounds.getY() + 1.0f,
                   fillBounds.getRight(), fillBounds.getY() + 1.0f, 1.0f);
    }
    else
    {
        // Inactive: linear-gradient(180deg, ${color}cc, ${color}99) with subtle glow
        
        // Subtle glow: 0 0 6px ${color}66
        g.setColour(effectColor.withAlpha(0.4f));
        g.fillRoundedRectangle(fillBounds.expanded(1.0f), STEP_RADIUS + 0.5f);
        
        // Gradient fill
        juce::ColourGradient fill(effectColor.withAlpha(0.8f), fillBounds.getTopLeft(),
                                  effectColor.withAlpha(0.6f), fillBounds.getBottomLeft(), false);
        g.setGradientFill(fill);
        g.fillRoundedRectangle(fillBounds, STEP_RADIUS);
        
        // Inner highlight: inset 0 1px 1px rgba(255, 255, 255, 0.2)
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawLine(fillBounds.getX(), fillBounds.getY() + 1.0f,
                   fillBounds.getRight(), fillBounds.getY() + 1.0f, 1.0f);
    }
}

void MetallicLookAndFeel::drawActiveStepBorder(juce::Graphics& g,
                                                juce::Rectangle<float> bounds,
                                                juce::Colour effectColor,
                                                float pulsePhase)
{
    // Pulsing effect: vary alpha based on pulse phase (0.0 - 1.0)
    float pulseAlpha = 0.6f + 0.4f * std::sin(pulsePhase * juce::MathConstants<float>::twoPi);
    
    // Outer border
    g.setColour(effectColor.withAlpha(pulseAlpha));
    g.drawRoundedRectangle(bounds, STEP_RADIUS, 2.0f);
    
    // Inner glow: inset 0 0 8px ${color}66
    g.setColour(effectColor.withAlpha(0.4f * pulseAlpha));
    g.fillRoundedRectangle(bounds.reduced(2.0f), STEP_RADIUS - 1.0f);
}

void MetallicLookAndFeel::drawStepNumber(juce::Graphics& g,
                                         int stepNum,
                                         juce::Rectangle<float> bounds)
{
    g.setFont(10.0f);
    
    // Shadow: text-shadow: 0 1px 0 rgba(0, 0, 0, 0.8)
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawText(juce::String(stepNum),
               bounds.translated(0, 1.0f).toNearestInt(),
               juce::Justification::topLeft);
    
    // Main text: color: #5a5a5d
    g.setColour(TEXT_DARK);
    g.drawText(juce::String(stepNum),
               bounds.toNearestInt(),
               juce::Justification::topLeft);
}

// ============================================================================
// TEXT EFFECTS
// ============================================================================

void MetallicLookAndFeel::drawEmbossedText(juce::Graphics& g,
                                           const juce::String& text,
                                           juce::Rectangle<int> bounds,
                                           juce::Justification justification)
{
    // Shadow: 0 2px 4px rgba(0, 0, 0, 0.8)
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.drawText(text, bounds.translated(0, 2), justification);
    
    // Highlight: 0 -1px 0 rgba(255, 255, 255, 0.1)
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawText(text, bounds.translated(0, -1), justification);
    
    // Main text
    g.setColour(TEXT_PRIMARY);
    g.drawText(text, bounds, justification);
}

// ============================================================================
// EFFECT INDICATOR DOT
// ============================================================================

void MetallicLookAndFeel::drawEffectIndicator(juce::Graphics& g,
                                              juce::Point<float> center,
                                              juce::Colour effectColor)
{
    float radius = 6.0f;
    
    // Outer glow: 0 0 6px ${color}99
    g.setColour(effectColor.withAlpha(0.6f));
    g.fillEllipse(center.x - radius - 2.0f, center.y - radius - 2.0f,
                  (radius + 2.0f) * 2.0f, (radius + 2.0f) * 2.0f);
    
    // Radial gradient: radial-gradient(circle, ${color}, ${color}dd)
    juce::ColourGradient radial(effectColor, center,
                                effectColor.withAlpha(0.87f),
                                juce::Point<float>(center.x + radius, center.y), true);
    g.setGradientFill(radial);
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    
    // Inner highlight: inset 0 1px 2px rgba(255, 255, 255, 0.2)
    float innerRadius = radius * 0.7f;
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.fillEllipse(center.x - innerRadius, center.y - innerRadius,
                  innerRadius * 2.0f, innerRadius * 2.0f);
}