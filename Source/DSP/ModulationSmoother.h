#pragma once

#include "StepSequencer.h"

//==============================================================================
struct ModulationSmoother
{
    void prepare(double sampleRate)
    {
        constexpr double rampSeconds = 0.01;

        filter.reset(sampleRate, rampSeconds);
        pan.reset(sampleRate, rampSeconds);
        delay.reset(sampleRate, rampSeconds);
        reverb.reset(sampleRate, rampSeconds);
        volume.reset(sampleRate, rampSeconds);

        filter.setCurrentAndTargetValue(0.5f);
        pan.setCurrentAndTargetValue(0.5f);
        delay.setCurrentAndTargetValue(0.5f);
        reverb.setCurrentAndTargetValue(0.5f);
        volume.setCurrentAndTargetValue(0.5f);
    }

    void setTargets(const ModulationValues& values)
    {
        filter.setTargetValue(values.filter);
        pan.setTargetValue(values.pan);
        delay.setTargetValue(values.delay);
        reverb.setTargetValue(values.reverb);
        volume.setTargetValue(values.volume);
    }

    ModulationValues advance(int numSamples)
    {
        if (numSamples > 1)
        {
            filter.skip(numSamples - 1);
            pan.skip(numSamples - 1);
            delay.skip(numSamples - 1);
            reverb.skip(numSamples - 1);
            volume.skip(numSamples - 1);
        }

        ModulationValues values;
        values.filter = filter.getNextValue();
        values.pan = pan.getNextValue();
        values.delay = delay.getNextValue();
        values.reverb = reverb.getNextValue();
        values.volume = volume.getNextValue();
        return values;
    }

private:
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> filter;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> pan;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> delay;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> reverb;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> volume;
};
