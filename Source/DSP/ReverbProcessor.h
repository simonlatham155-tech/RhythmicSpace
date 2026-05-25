#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Reverb processor with modulation
 */
class ReverbProcessor
{
public:
    ReverbProcessor();
    ~ReverbProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 float modulation, float size, float damping, float mix);
    
    void reset();

private:
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> dryBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbProcessor)
};
