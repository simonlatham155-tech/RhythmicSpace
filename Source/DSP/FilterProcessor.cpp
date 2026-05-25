#include "FilterProcessor.h"

//==============================================================================
FilterProcessor::FilterProcessor()
{
}

FilterProcessor::~FilterProcessor()
{
}

//==============================================================================
void FilterProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    filter.prepare(spec);
    filter.reset();
    
    dryBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
}

void FilterProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                               float modulation, float cutoff, float resonance,
                               float type, float mix)
{
    auto& block = context.getOutputBlock();
    
    // Store dry signal for mixing
    dryBuffer.setSize(static_cast<int>(block.getNumChannels()), static_cast<int>(block.getNumSamples()), false, false, true);
    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        dryBuffer.copyFrom(static_cast<int>(ch), 0, block.getChannelPointer(ch), static_cast<int>(block.getNumSamples()));
    
    // Apply modulation to cutoff frequency
    // Modulation ranges from 20Hz to 20kHz
    float modulatedCutoff = 20.0f + (20000.0f - 20.0f) * modulation;
    modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff * (cutoff / 1000.0f));
    
    // Set filter type
    int filterType = static_cast<int>(type);
    switch (filterType)
    {
        case LowPass:
            filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
        case HighPass:
            filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
            break;
        case BandPass:
            filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
    }
    
    // Update filter parameters
    filter.setCutoffFrequency(modulatedCutoff);
    filter.setResonance(resonance);
    
    // Process
    filter.process(context);
    
    // Mix dry and wet signals
    if (mix < 1.0f)
    {
        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            auto* wetData = block.getChannelPointer(ch);
            auto* dryData = dryBuffer.getReadPointer(static_cast<int>(ch));
            
            for (size_t i = 0; i < block.getNumSamples(); ++i)
                wetData[i] = dryData[i] * (1.0f - mix) + wetData[i] * mix;
        }
    }
}

void FilterProcessor::reset()
{
    filter.reset();
}
