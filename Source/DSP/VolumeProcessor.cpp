#include "VolumeProcessor.h"

//==============================================================================
VolumeProcessor::VolumeProcessor()
{
}

VolumeProcessor::~VolumeProcessor()
{
}

//==============================================================================
void VolumeProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    gain.prepare(spec);
    reset();
}

void VolumeProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                               float modulation, float amount)
{
    // Apply modulation to volume
    // Modulation ranges from 0.0 to 1.0
    // Amount controls how much the sequencer affects volume
    float volumeGain = modulation * amount + (1.0f - amount);
    volumeGain = juce::jlimit(0.0f, 2.0f, volumeGain);
    
    // Convert to decibels for natural volume control
    float gainDB = juce::Decibels::gainToDecibels(volumeGain);
    gain.setGainDecibels(gainDB);
    
    // Process
    gain.process(context);
}

void VolumeProcessor::reset()
{
    gain.reset();
}
