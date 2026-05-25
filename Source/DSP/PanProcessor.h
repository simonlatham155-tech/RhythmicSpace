#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Auto-pan processor with modulation
 */
class PanProcessor
{
public:
    PanProcessor();
    ~PanProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 float modulation, float width, float rate);
    
    void reset();

private:
    juce::dsp::Panner<float> panner;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanProcessor)
};
