#pragma once

#include <JuceHeader.h>
#include "../Data/MIDIControllerMap.h"

//==============================================================================
/**
 * Component for MIDI learn functionality
 */
class MIDILearnComponent : public juce::Component,
                           private juce::Button::Listener,
                           private juce::ComboBox::Listener,
                           private juce::Timer
{
public:
    MIDILearnComponent(MIDIControllerMap& map);
    ~MIDILearnComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Call this when MIDI is received to show activity
    void flashMIDIActivity() { midiActivityCounter = 10; }

private:
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void timerCallback() override;
    
    MIDIControllerMap& midiMap;
    
    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::ComboBox templateCombo;
    juce::TextButton clearButton;
    juce::TextButton learnButton;
    
    bool blinkState = false;
    int midiActivityCounter = 0; // Counts down to show MIDI activity
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDILearnComponent)
};