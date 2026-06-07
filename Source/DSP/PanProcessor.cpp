#include "PanProcessor.h"

//==============================================================================
PanProcessor::PanProcessor()
{
}

PanProcessor::~PanProcessor()
{
}

//==============================================================================
void PanProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    panner.prepare(spec);
    reset();
}

void PanProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                            float modulation, float width, float rate)
{
    auto& block = context.getOutputBlock();
    const auto numSamples = static_cast<int>(block.getNumSamples());

    const float lfoHz = rate * 8.0f;
    const float lfoOffset = std::sin(lfoPhase) * width * rate;
    float panPosition = (modulation - 0.5f) * 2.0f * width + lfoOffset;
    panPosition = juce::jlimit(-1.0f, 1.0f, panPosition);

    panner.setPan(panPosition);
    panner.setRule(juce::dsp::PannerRule::balanced);
    panner.process(context);

    if (sampleRate > 0.0 && numSamples > 0)
        lfoPhase += juce::MathConstants<float>::twoPi * lfoHz * (float) numSamples / (float) sampleRate;

    if (lfoPhase > juce::MathConstants<float>::twoPi)
        lfoPhase -= juce::MathConstants<float>::twoPi;
}

void PanProcessor::reset()
{
    panner.reset();
    lfoPhase = 0.0f;
}
