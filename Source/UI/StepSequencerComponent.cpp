#include "StepSequencerComponent.h"

StepSequencerComponent::StepSequencerComponent(const juce::String& labelText,
                                               MetallicLookAndFeel::EffectType type)
    : label(labelText), effectType(type)
{
    // Initialize all steps to 50% (middle value)
    stepValues.fill(0.5f);
    
    // Set effect-specific color
    effectColor = MetallicLookAndFeel::getEffectColour(effectType);
    
    // Start pulse animation timer (30 FPS)
    startTimerHz(30);
}

// ============================================================================
// STEP DATA
// ============================================================================

void StepSequencerComponent::setStepValue(int stepIndex, float value)
{
    if (stepIndex >= 0 && stepIndex < NUM_STEPS)
    {
        const float clampedValue = juce::jlimit(MIN_VALUE, MAX_VALUE, value);

        if (juce::approximatelyEqual(stepValues[stepIndex], clampedValue))
            return;

        stepValues[stepIndex] = clampedValue;
        repaint();
        
        if (onStepChanged)
            onStepChanged(stepIndex, stepValues[stepIndex]);
    }
}

float StepSequencerComponent::getStepValue(int stepIndex) const
{
    if (stepIndex >= 0 && stepIndex < NUM_STEPS)
        return stepValues[stepIndex];
    return 0.0f;
}

void StepSequencerComponent::setAllSteps(const std::vector<float>& values)
{
    for (size_t i = 0; i < values.size() && i < NUM_STEPS; ++i)
        stepValues[i] = juce::jlimit(MIN_VALUE, MAX_VALUE, values[i]);
    
    repaint();
}

std::vector<float> StepSequencerComponent::getAllSteps() const
{
    return std::vector<float>(stepValues.begin(), stepValues.end());
}

// ============================================================================
// PLAYBACK STATE
// ============================================================================

void StepSequencerComponent::setCurrentStep(int step)
{
    if (currentStep != step)
    {
        currentStep = step % NUM_STEPS;
        repaint();
    }
}

// ============================================================================
// APPEARANCE
// ============================================================================

void StepSequencerComponent::setEffectColor(juce::Colour color)
{
    effectColor = color;
    repaint();
}

void StepSequencerComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // ========================================================================
    // LABEL AND INDICATOR
    // ========================================================================
    
    auto labelBounds = bounds.removeFromTop(LABEL_HEIGHT);
    auto labelTextBounds = labelBounds.withTrimmedRight(20);
    auto indicatorBounds = labelBounds.removeFromRight(20);
    
    // Draw label text
    g.setFont(11.0f);
    g.setColour(MetallicLookAndFeel::TEXT_TERTIARY);
    g.drawText(label.toUpperCase(), labelTextBounds, 
               juce::Justification::centredLeft);
    
    // Draw effect color indicator dot
    auto indicatorCenter = indicatorBounds.getCentre().toFloat();
    MetallicLookAndFeel::drawEffectIndicator(g, indicatorCenter, effectColor);
    
    // ========================================================================
    // STEP BARS
    // ========================================================================
    
    for (int i = 0; i < NUM_STEPS; ++i)
    {
        auto stepBounds = getStepBounds(i);
        
        // Background
        MetallicLookAndFeel::drawStepBarBackground(g, stepBounds);
        
        // Value bar
        float value = stepValues[i];
        bool isActive = (i == currentStep);
        
        MetallicLookAndFeel::drawStepBarFill(g, stepBounds, effectColor, value, isActive);
        
        // Active step border (pulsing)
        if (isActive)
        {
            MetallicLookAndFeel::drawActiveStepBorder(g, stepBounds, effectColor, pulsePhase);
        }
        
        // Step number
        auto numberBounds = stepBounds.reduced(2.0f).withHeight(12.0f);
        MetallicLookAndFeel::drawStepNumber(g, i + 1, numberBounds);
    }
}

void StepSequencerComponent::resized()
{
    // Layout is handled in paint()
}

// ============================================================================
// INTERACTION
// ============================================================================

void StepSequencerComponent::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    updateStepFromMouse(event);
}

void StepSequencerComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
        updateStepFromMouse(event);
}

void StepSequencerComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
}

// ============================================================================
// HELPER METHODS
// ============================================================================

int StepSequencerComponent::getStepIndexFromX(int x) const
{
    auto bounds = getLocalBounds().withTrimmedTop(LABEL_HEIGHT);
    float stepWidth = bounds.getWidth() / static_cast<float>(NUM_STEPS);
    int index = static_cast<int>(x / stepWidth);
    return juce::jlimit(0, NUM_STEPS - 1, index);
}

float StepSequencerComponent::getValueFromY(int y) const
{
    auto bounds = getLocalBounds().withTrimmedTop(LABEL_HEIGHT);
    float relativeY = y - bounds.getY();
    float percentage = 1.0f - (relativeY / bounds.getHeight());
    return juce::jlimit(MIN_VALUE, MAX_VALUE, percentage);
}

juce::Rectangle<float> StepSequencerComponent::getStepBounds(int stepIndex) const
{
    auto bounds = getLocalBounds().withTrimmedTop(LABEL_HEIGHT).toFloat();
    float stepWidth = bounds.getWidth() / static_cast<float>(NUM_STEPS);
    float gap = 2.0f;
    
    float x = bounds.getX() + stepIndex * stepWidth + gap * 0.5f;
    float width = stepWidth - gap;
    
    return juce::Rectangle<float>(x, bounds.getY(), width, bounds.getHeight());
}

void StepSequencerComponent::updateStepFromMouse(const juce::MouseEvent& event)
{
    int stepIndex = getStepIndexFromX(event.x);
    float value = getValueFromY(event.y);
    setStepValue(stepIndex, value);
}

void StepSequencerComponent::timerCallback()
{
    // Update pulse phase for animation (cycles every 2 seconds)
    pulsePhase += 1.0f / 60.0f;  // Increment by ~1/60th per frame at 30 FPS
    if (pulsePhase > 1.0f)
        pulsePhase -= 1.0f;
    
    // Only repaint if we have an active step
    if (currentStep >= 0 && currentStep < NUM_STEPS)
        repaint();
}
