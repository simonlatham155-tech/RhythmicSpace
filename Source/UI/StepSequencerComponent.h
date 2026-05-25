#pragma once

#include <JuceHeader.h>
#include "MetallicLookAndFeel.h"

/**
 * StepSequencerComponent - 16-step sequencer lane with color-coded effect modulation
 * 
 * Features:
 * - Click and drag to draw automation patterns
 * - Visual feedback for current playback position
 * - Effect-specific color coding
 * - Pulsing active step indicator
 * - Step number labels
 */
class StepSequencerComponent : public juce::Component, private juce::Timer
{
public:
    StepSequencerComponent(const juce::String& labelText, 
                          MetallicLookAndFeel::EffectType effectType);
    ~StepSequencerComponent() override = default;

    // ============================================================================
    // STEP DATA
    // ============================================================================
    
    void setStepValue(int stepIndex, float value);
    float getStepValue(int stepIndex) const;
    void setAllSteps(const std::vector<float>& values);
    std::vector<float> getAllSteps() const;
    
    // ============================================================================
    // PLAYBACK STATE
    // ============================================================================
    
    void setCurrentStep(int step);
    int getCurrentStep() const { return currentStep; }
    
    // ============================================================================
    // APPEARANCE
    // ============================================================================
    
    void setEffectColor(juce::Colour color);
    juce::Colour getEffectColor() const { return effectColor; }
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // ============================================================================
    // INTERACTION
    // ============================================================================
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    
    // ============================================================================
    // VALUE CHANGE CALLBACK
    // ============================================================================
    
    std::function<void(int stepIndex, float value)> onStepChanged;

private:
    static constexpr int NUM_STEPS = 16;
    static constexpr float MIN_VALUE = 0.0f;
    static constexpr float MAX_VALUE = 1.0f;
    static constexpr int STEP_HEIGHT = 80;
    static constexpr int LABEL_HEIGHT = 24;
    
    // Step values (0.0 - 1.0)
    std::array<float, NUM_STEPS> stepValues;
    
    // Playback state
    int currentStep = 0;
    float pulsePhase = 0.0f;  // For pulsing animation (0.0 - 1.0)
    
    // Appearance
    juce::String label;
    MetallicLookAndFeel::EffectType effectType;
    juce::Colour effectColor;
    
    // Interaction state
    bool isDragging = false;
    
    // ============================================================================
    // HELPER METHODS
    // ============================================================================
    
    int getStepIndexFromX(int x) const;
    float getValueFromY(int y) const;
    juce::Rectangle<float> getStepBounds(int stepIndex) const;
    void updateStepFromMouse(const juce::MouseEvent& event);
    
    // Timer for pulsing animation
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepSequencerComponent)
};
