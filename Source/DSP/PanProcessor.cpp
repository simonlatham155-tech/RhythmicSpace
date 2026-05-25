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
    panner.prepare(spec);
    reset();
}

void PanProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                            float modulation, float width, float rate)
{
    // Apply modulation to pan position
    // Modulation value ranges from 0.0 to 1.0
    // Convert to pan range -1.0 (left) to 1.0 (right)
    float panPosition = (modulation - 0.5f) * 2.0f * width;
    panPosition = juce::jlimit(-1.0f, 1.0f, panPosition);
    
    // Set pan rule (balanced, linear, or sin/cos)
    panner.setPan(panPosition);
    panner.setRule(juce::dsp::PannerRule::balanced);
    
    // Process
    panner.process(context);
}

void PanProcessor::reset()
{
    panner.reset();
}
