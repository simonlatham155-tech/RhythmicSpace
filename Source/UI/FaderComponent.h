#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Labeled vertical fader component
 */
class FaderComponent : public juce::Component
{
public:
    FaderComponent(const juce::String& labelText,
                   juce::AudioProcessorValueTreeState& vts,
                   const juce::String& parameterID);
    ~FaderComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider fader;
    juce::Label label;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FaderComponent)
};
