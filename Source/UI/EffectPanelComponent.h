#pragma once

#include <JuceHeader.h>
#include "KnobComponent.h"

//==============================================================================
/**
 * Tabbed effect parameter panel
 */
class EffectPanelComponent : public juce::Component
{
public:
    EffectPanelComponent(juce::AudioProcessorValueTreeState& vts);
    ~EffectPanelComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Set MIDI learn callback for all knobs
    void setMIDILearnCallback(std::function<void(const juce::String&)> callback);

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    // Tabs
    juce::TabbedComponent tabbedComponent;
    
    // Effect parameter components
    std::unique_ptr<juce::Component> createFilterPanel();
    std::unique_ptr<juce::Component> createDelayPanel();
    std::unique_ptr<juce::Component> createReverbPanel();
    std::unique_ptr<juce::Component> createPanPanel();
    std::unique_ptr<juce::Component> createVolumePanel();
    
    // Store all knobs for MIDI learn callback
    std::vector<KnobComponent*> allKnobs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectPanelComponent)
};
