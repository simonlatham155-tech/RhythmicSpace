#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Volume envelope processor with modulation
 */
class VolumeProcessor
{
public:
    VolumeProcessor();
    ~VolumeProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 float modulation, float amount);
    
    void reset();

private:
    juce::dsp::Gain<float> gain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeProcessor)
};
