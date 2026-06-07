#include "FilterProcessor.h"

//==============================================================================
FilterProcessor::FilterProcessor()
{
}

FilterProcessor::~FilterProcessor()
{
}

//==============================================================================
juce::dsp::StateVariableTPTFilterType FilterProcessor::getFilterType(int filterTypeIndex)
{
    switch (juce::jlimit(0, 2, filterTypeIndex))
    {
        case HighPass: return juce::dsp::StateVariableTPTFilterType::highpass;
        case BandPass: return juce::dsp::StateVariableTPTFilterType::bandpass;
        case LowPass:
        default:       return juce::dsp::StateVariableTPTFilterType::lowpass;
    }
}

void FilterProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    for (auto& filter : filters)
    {
        filter.prepare(monoSpec);
        filter.reset();
    }

    dryBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
}

void FilterProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                               float modulation, float cutoff, float resonance,
                               int filterTypeIndex, float mix)
{
    auto& block = context.getOutputBlock();
    const auto filterType = getFilterType(filterTypeIndex);

    dryBuffer.setSize(static_cast<int>(block.getNumChannels()),
                     static_cast<int>(block.getNumSamples()),
                     false, false, true);

    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        dryBuffer.copyFrom(static_cast<int>(ch), 0, block.getChannelPointer(ch), static_cast<int>(block.getNumSamples()));

    float modulatedCutoff = 20.0f + (20000.0f - 20.0f) * modulation;
    modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff * (cutoff / 1000.0f));
    modulatedCutoff = juce::jmin(modulatedCutoff, static_cast<float>(sampleRate * 0.45));

    const size_t numChannels = juce::jmin(block.getNumChannels(), filters.size());

    for (size_t ch = 0; ch < numChannels; ++ch)
    {
        auto& filter = filters[ch];
        filter.setType(filterType);
        filter.setCutoffFrequency(modulatedCutoff);
        filter.setResonance(resonance);

        auto channelBlock = block.getSingleChannelBlock(ch);
        juce::dsp::ProcessContextReplacing<float> channelContext(channelBlock);
        filter.process(channelContext);
    }

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
    for (auto& filter : filters)
        filter.reset();
}
