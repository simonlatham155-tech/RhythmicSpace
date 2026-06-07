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
    reset();
}

void ReverbProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                               float modulation, float size, float damping, float mix)
{
    const float modulatedSize = size * modulation;

    juce::dsp::Reverb::Parameters params;
    params.roomSize = juce::jlimit(0.0f, 1.0f, modulatedSize);
    params.damping = damping;
    params.wetLevel = mix;
    params.dryLevel = 1.0f - mix;
    params.width = 1.0f;
    params.freezeMode = 0.0f;

    reverb.setParameters(params);
    reverb.process(context);
}

void ReverbProcessor::reset()
{
    reverb.reset();
}
