#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Stereo delay processor with feedback and modulation
 */
class DelayProcessor
{
public:
    DelayProcessor();
    ~DelayProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 float modulation, float delayTime, float feedback, float mix);
    
    void reset();

private:
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineL;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineR;
    
    juce::AudioBuffer<float> dryBuffer;
    
    double sampleRate = 44100.0;
    float lastFeedbackL = 0.0f;
    float lastFeedbackR = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayProcessor)
};
