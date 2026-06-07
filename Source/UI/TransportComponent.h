#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

//==============================================================================
/**
 * Transport controls: Play/Stop, BPM, Randomize
 */
class TransportComponent : public juce::Component,
                           private juce::Button::Listener,
                           private juce::Slider::Listener
{
public:
    TransportComponent(RhythmicSpaceAudioProcessor& proc);
    ~TransportComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void syncFromProcessor();

private:
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    
    RhythmicSpaceAudioProcessor& processor;
    
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton randomizeButton;
    juce::ToggleButton syncButton;
    juce::Slider bpmSlider;
    juce::Label bpmLabel;
    
    void updateBpmControlState();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
