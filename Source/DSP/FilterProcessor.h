#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Multi-mode filter processor with step sequencer modulation
 */
class FilterProcessor
{
public:
    FilterProcessor();
    ~FilterProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 float modulation, float cutoff, float resonance, 
                 float type, float mix);
    
    void reset();

private:
    enum FilterType
    {
        LowPass = 0,
        HighPass,
        BandPass
    };
    
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::AudioBuffer<float> dryBuffer;
    
    double sampleRate = 44100.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterProcessor)
};
