#include "ReverbProcessor.h"

//==============================================================================
ReverbProcessor::ReverbProcessor()
{
}

ReverbProcessor::~ReverbProcessor()
{
}

//==============================================================================
void ReverbProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare(spec);
    dryBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
    reset();
}

void ReverbProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                               float modulation, float size, float damping, float mix)
{
    auto& block = context.getOutputBlock();
    
    // Store dry signal
    dryBuffer.setSize(static_cast<int>(block.getNumChannels()), static_cast<int>(block.getNumSamples()), false, false, true);
    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        dryBuffer.copyFrom(static_cast<int>(ch), 0, block.getChannelPointer(ch), static_cast<int>(block.getNumSamples()));
    
    // Apply modulation to size
    float modulatedSize = size * modulation;
    
    // Set reverb parameters
    juce::dsp::Reverb::Parameters params;
    params.roomSize = juce::jlimit(0.0f, 1.0f, modulatedSize);
    params.damping = damping;
    params.wetLevel = mix;
    params.dryLevel = 1.0f - mix;
    params.width = 1.0f;
    params.freezeMode = 0.0f;
    
    reverb.setParameters(params);
    
    // Process
    reverb.process(context);
    
    // Manual dry/wet mix for more control
    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
    {
        auto* wetData = block.getChannelPointer(ch);
        auto* dryData = dryBuffer.getReadPointer(static_cast<int>(ch));
        
        for (size_t i = 0; i < block.getNumSamples(); ++i)
            wetData[i] = dryData[i] * (1.0f - mix) + wetData[i] * mix;
    }
}

void ReverbProcessor::reset()
{
    reverb.reset();
}
