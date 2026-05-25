#pragma once

#include <JuceHeader.h>

/**
 * MetallicLookAndFeel - Professional hardware-style visual design
 * 
 * Exact color match to web prototype with:
 * - 5 unique step sequencer colors per effect
 * - Multi-stop metallic gradients
 * - Recessed panels with inner shadows
 * - 3D button beveling
 * - Embossed text effects
 */
class MetallicLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MetallicLookAndFeel();
    ~MetallicLookAndFeel() override = default;

    // ============================================================================
    // EFFECT COLORS (Exact web match)
    // ============================================================================
    
    enum class EffectType
    {
        Filter,   // Orange: #ff6b35
        Pan,      // Cyan: #00d4ff
        Delay,    // Green: #00ff88
        Reverb,   // Magenta: #ff00ff
        Volume    // Amber: #ffaa00
    };
    
    static juce::Colour getEffectColour(EffectType type);
    
    // ============================================================================
    // PANEL RENDERING
    // ============================================================================
    
    static void drawMainPanel(juce::Graphics& g, juce::Rectangle<int> bounds);
    static void drawHeader(juce::Graphics& g, juce::Rectangle<int> bounds);
    static void drawRecessedPanel(juce::Graphics& g, juce::Rectangle<int> bounds);
    static void drawBadge(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    // New beveled hardware-style panels
    static void drawOuterChassis(juce::Graphics& g, juce::Rectangle<int> bounds);
    static void drawInnerRecessedPanel(juce::Graphics& g, juce::Rectangle<int> bounds);
    static void drawDeeplyRecessedSection(juce::Graphics& g, juce::Rectangle<int> bounds);
    static void drawCornerScrew(juce::Graphics& g, juce::Point<float> center, float radius);
    static void drawEmbossedBadge(juce::Graphics& g, juce::Rectangle<int> bounds, 
                                   const juce::String& lightText, const juce::String& boldText);
    
    // ============================================================================
    // BUTTON RENDERING
    // ============================================================================
    
    void drawButtonBackground(juce::Graphics& g,
                            juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool isMouseOverButton,
                            bool isButtonDown) override;
    
    // ============================================================================
    // KNOB (ROTARY SLIDER) RENDERING
    // ============================================================================
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider& slider) override;
    
    static void drawNormalButton(juce::Graphics& g, juce::Rectangle<float> bounds,
                                bool isOver, bool isDown);
    static void drawAccentButton(juce::Graphics& g, juce::Rectangle<float> bounds,
                                bool isOver, bool isDown);
    
    // ============================================================================
    // STEP SEQUENCER RENDERING
    // ============================================================================
    
    static void drawStepBarBackground(juce::Graphics& g, juce::Rectangle<float> bounds);
    
    static void drawStepBarFill(juce::Graphics& g, 
                               juce::Rectangle<float> bounds,
                               juce::Colour effectColor,
                               float value,
                               bool isActive);
    
    static void drawActiveStepBorder(juce::Graphics& g, 
                                    juce::Rectangle<float> bounds,
                                    juce::Colour effectColor,
                                    float pulsePhase);
    
    static void drawStepNumber(juce::Graphics& g, 
                              int stepNum,
                              juce::Rectangle<float> bounds);
    
    // ============================================================================
    // TEXT EFFECTS
    // ============================================================================
    
    static void drawEmbossedText(juce::Graphics& g,
                                const juce::String& text,
                                juce::Rectangle<int> bounds,
                                juce::Justification justification = juce::Justification::centred);
    
    // ============================================================================
    // EFFECT INDICATOR DOT
    // ============================================================================
    
    static void drawEffectIndicator(juce::Graphics& g, 
                                   juce::Point<float> center,
                                   juce::Colour effectColor);
    
    // ============================================================================
    // COLOR PALETTE
    // ============================================================================
    
    // Step sequencer effect colors
    static const juce::Colour FILTER_COLOR;
    static const juce::Colour PAN_COLOR;
    static const juce::Colour DELAY_COLOR;
    static const juce::Colour REVERB_COLOR;
    static const juce::Colour VOLUME_COLOR;
    
    // Metallic panel colors
    static const juce::Colour METAL_DARK;
    static const juce::Colour METAL_MID_DARK;
    static const juce::Colour METAL_MID;
    static const juce::Colour METAL_MID_LIGHT;
    
    // Recessed/dark areas
    static const juce::Colour RECESS_DARK;
    static const juce::Colour RECESS_MID;
    static const juce::Colour RECESS_LIGHT;
    
    // Border colors
    static const juce::Colour BORDER_DARK;
    static const juce::Colour BORDER_MID;
    
    // Text colors
    static const juce::Colour TEXT_PRIMARY;
    static const juce::Colour TEXT_SECONDARY;
    static const juce::Colour TEXT_TERTIARY;
    static const juce::Colour TEXT_DISABLED;
    static const juce::Colour TEXT_DARK;
    
    // Accent colors
    static const juce::Colour ACCENT_BRIGHT;
    static const juce::Colour ACCENT_MID;
    static const juce::Colour ACCENT_MEDIUM;
    static const juce::Colour ACCENT_DARK;
    
    // ============================================================================
    // DIMENSIONS
    // ============================================================================
    
    static constexpr float PANEL_RADIUS = 16.0f;
    static constexpr float SECTION_RADIUS = 12.0f;
    static constexpr float BUTTON_RADIUS = 4.0f;
    static constexpr float STEP_RADIUS = 2.0f;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetallicLookAndFeel)
};
