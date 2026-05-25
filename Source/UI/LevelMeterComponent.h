#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Vertical level meter with peak hold
 */
class LevelMeterComponent : public juce::Component,
                            private juce::Timer
{
public:
    LevelMeterComponent();
    ~LevelMeterComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setLevel(float newLevel);
    
private:
    void timerCallback() override;
    
    float currentLevel = 0.0f;
    float peakLevel = 0.0f;
    int peakHoldCounter = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterComponent)
};
