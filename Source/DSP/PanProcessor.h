#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Stereo pan processor with step sequencer modulation and optional LFO rate
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
    double sampleRate = 44100.0;
    float lfoPhase = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanProcessor)
};
