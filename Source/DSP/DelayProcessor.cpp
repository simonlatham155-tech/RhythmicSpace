#include "DelayProcessor.h"

//==============================================================================
DelayProcessor::DelayProcessor()
{
}

DelayProcessor::~DelayProcessor()
{
}

//==============================================================================
void DelayProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    // Maximum delay of 2 seconds
    delayLineL.prepare(spec);
    delayLineR.prepare(spec);
    delayLineL.setMaximumDelayInSamples(static_cast<int>(sampleRate * 2.0));
    delayLineR.setMaximumDelayInSamples(static_cast<int>(sampleRate * 2.0));
    
    dryBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
    
    reset();
}

void DelayProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                              float modulation, float delayTime, float feedback, float mix)
{
    auto& block = context.getOutputBlock();
    
    // Store dry signal
    dryBuffer.setSize(static_cast<int>(block.getNumChannels()), static_cast<int>(block.getNumSamples()), false, false, true);
    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        dryBuffer.copyFrom(static_cast<int>(ch), 0, block.getChannelPointer(ch), static_cast<int>(block.getNumSamples()));
    
    // Apply modulation to delay time (0-2000ms)
    float modulatedDelayTime = delayTime * modulation;
    float delaySamples = (modulatedDelayTime / 1000.0f) * static_cast<float>(sampleRate);
    delaySamples = juce::jlimit(1.0f, static_cast<float>(sampleRate * 2.0), delaySamples);
    
    delayLineL.setDelay(delaySamples);
    delayLineR.setDelay(delaySamples);
    
    // Process each channel
    auto* leftData = block.getChannelPointer(0);
    auto* rightData = block.getNumChannels() > 1 ? block.getChannelPointer(1) : leftData;
    
    for (size_t i = 0; i < block.getNumSamples(); ++i)
    {
        // Get delayed samples
        float delayedL = delayLineL.popSample(0);
        float delayedR = delayLineR.popSample(0);
        
        // Apply feedback
        float inputL = leftData[i] + delayedL * feedback;
        float inputR = rightData[i] + delayedR * feedback;
        
        // Push to delay line
        delayLineL.pushSample(0, inputL);
        delayLineR.pushSample(0, inputR);
        
        // Mix dry and wet
        leftData[i] = dryBuffer.getSample(0, static_cast<int>(i)) * (1.0f - mix) + delayedL * mix;
        if (block.getNumChannels() > 1)
            rightData[i] = dryBuffer.getSample(1, static_cast<int>(i)) * (1.0f - mix) + delayedR * mix;
    }
}

void DelayProcessor::reset()
{
    delayLineL.reset();
    delayLineR.reset();
    lastFeedbackL = 0.0f;
    lastFeedbackR = 0.0f;
}
