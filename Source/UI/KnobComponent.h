#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Labeled rotary knob component
 */
class KnobComponent : public juce::Component,
                      private juce::Slider::Listener
{
public:
    KnobComponent(const juce::String& labelText,
                  juce::AudioProcessorValueTreeState& vts,
                  const juce::String& parameterID);
    ~KnobComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    
    // MIDI Learn callback
    std::function<void(const juce::String&)> onMIDILearnRequest;
    
    juce::String getParameterID() const { return paramID; }

private:
    void sliderValueChanged(juce::Slider* slider) override {}
    
    juce::Slider knob;
    juce::Label label;
    juce::String paramID;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobComponent)
};
