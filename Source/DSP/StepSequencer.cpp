#include "StepSequencer.h"

//==============================================================================
StepSequencer::StepSequencer()
{
    for (auto& paramSteps : steps)
    {
        for (auto& step : paramSteps)
            step.store(0.5f);
    }
}

StepSequencer::~StepSequencer()
{
}

//==============================================================================
void StepSequencer::prepare(double newSampleRate, double newBPM)
{
    sampleRate.store(newSampleRate);
    bpm.store(newBPM);
    updateStepTiming();
    reset();
}

void StepSequencer::updateStepTiming()
{
    const double safeBpm = juce::jlimit(1.0, 999.0, bpm.load());
    const double safeSampleRate = juce::jmax(1.0, sampleRate.load());
    bpm.store(safeBpm);
    sampleRate.store(safeSampleRate);

    const double beatsPerSecond = safeBpm / 60.0;
    const double sixteenthNotesPerSecond = beatsPerSecond * 4.0;
    const double samplesPerSixteenthNote = safeSampleRate / sixteenthNotesPerSecond;

    samplesPerStep.store(juce::jmax(1, (int) std::round(samplesPerSixteenthNote)));
}

int StepSequencer::getSamplesUntilNextStep() const
{
    return juce::jmax(1, samplesPerStep.load() - sampleCounter.load());
}

void StepSequencer::advance(int numSamples)
{
    const int stepLength = samplesPerStep.load();
    int counter = sampleCounter.load() + numSamples;
    int step = currentStep.load();

    while (counter >= stepLength)
    {
        counter -= stepLength;
        step = (step + 1) % NUM_STEPS;
    }

    sampleCounter.store(counter);
    currentStep.store(step);
}

void StepSequencer::reset()
{
    currentStep.store(0);
    sampleCounter.store(0);
}

void StepSequencer::syncToHostPpq(double ppqPosition)
{
    const double sixteenthPosition = ppqPosition * 4.0;
    const auto wholeSteps = static_cast<int64_t>(std::floor(sixteenthPosition));

    int step = static_cast<int>(wholeSteps % NUM_STEPS);
    if (step < 0)
        step += NUM_STEPS;

    currentStep.store(step);

    const double fraction = sixteenthPosition - std::floor(sixteenthPosition);
    const int stepLength = samplesPerStep.load();
    sampleCounter.store(juce::jlimit(0, juce::jmax(0, stepLength - 1),
                                       static_cast<int>(std::round(fraction * stepLength))));
}

//==============================================================================
void StepSequencer::setBPM(double newBPM)
{
    bpm.store(juce::jlimit(60.0, 240.0, newBPM));
    updateStepTiming();
}

void StepSequencer::setStepValue(int parameter, int step, float value)
{
    if (parameter >= 0 && parameter < NumParameters && step >= 0 && step < NUM_STEPS)
        steps[parameter][step].store(juce::jlimit(0.0f, 1.0f, value));
}

float StepSequencer::getStepValue(int parameter, int step) const
{
    if (parameter >= 0 && parameter < NumParameters && step >= 0 && step < NUM_STEPS)
        return steps[parameter][step].load();

    return 0.5f;
}

std::array<float, 16> StepSequencer::getSteps(int parameter) const
{
    if (parameter >= 0 && parameter < NumParameters)
    {
        std::array<float, 16> values;
        for (int step = 0; step < NUM_STEPS; ++step)
            values[step] = steps[parameter][step].load();
        return values;
    }

    std::array<float, 16> defaultSteps;
    defaultSteps.fill(0.5f);
    return defaultSteps;
}

int StepSequencer::getCurrentStep() const
{
    return currentStep.load();
}

ModulationValues StepSequencer::getCurrentModulationValues() const
{
    const int step = juce::jlimit(0, NUM_STEPS - 1, currentStep.load());

    ModulationValues values;
    values.filter = steps[FilterParam][step].load();
    values.pan = steps[PanParam][step].load();
    values.delay = steps[DelayParam][step].load();
    values.reverb = steps[ReverbParam][step].load();
    values.volume = steps[VolumeParam][step].load();
    return values;
}

//==============================================================================
void StepSequencer::randomizeParameter(int parameter)
{
    if (parameter >= 0 && parameter < NumParameters)
    {
        juce::Random random;

        for (auto& step : steps[parameter])
            step.store(random.nextFloat());
    }
}

void StepSequencer::randomizeAll()
{
    for (int i = 0; i < NumParameters; ++i)
        randomizeParameter(i);
}

void StepSequencer::clearParameter(int parameter)
{
    if (parameter >= 0 && parameter < NumParameters)
        for (auto& step : steps[parameter])
            step.store(0.5f);
}

void StepSequencer::clearAll()
{
    for (auto& paramSteps : steps)
    {
        for (auto& step : paramSteps)
            step.store(0.5f);
    }
}

//==============================================================================
void StepSequencer::loadPreset(const Preset& preset)
{
    for (int step = 0; step < NUM_STEPS; ++step)
    {
        steps[FilterParam][step].store(preset.filterSteps[step]);
        steps[PanParam][step].store(preset.panSteps[step]);
        steps[DelayParam][step].store(preset.delaySteps[step]);
        steps[ReverbParam][step].store(preset.reverbSteps[step]);
        steps[VolumeParam][step].store(preset.volumeSteps[step]);
    }

    setBPM(preset.bpm);
}

void StepSequencer::savePreset(Preset& preset) const
{
    for (int step = 0; step < NUM_STEPS; ++step)
    {
        preset.filterSteps[step] = steps[FilterParam][step].load();
        preset.panSteps[step] = steps[PanParam][step].load();
        preset.delaySteps[step] = steps[DelayParam][step].load();
        preset.reverbSteps[step] = steps[ReverbParam][step].load();
        preset.volumeSteps[step] = steps[VolumeParam][step].load();
    }

    preset.bpm = bpm.load();
}

//==============================================================================
void StepSequencer::saveState(juce::XmlElement& xml) const
{
    xml.setAttribute("bpm", bpm.load());
    xml.setAttribute("currentStep", currentStep.load());

    const char* paramNames[] = { "filter", "pan", "delay", "reverb", "volume" };

    for (int param = 0; param < NumParameters; ++param)
    {
        juce::String stepData;

        for (int step = 0; step < NUM_STEPS; ++step)
        {
            if (step > 0)
                stepData += ",";

            stepData += juce::String(steps[param][step].load(), 4);
        }

        xml.setAttribute(paramNames[param], stepData);
    }
}

void StepSequencer::loadState(const juce::XmlElement& xml)
{
    bpm.store(juce::jlimit(60.0, 240.0, xml.getDoubleAttribute("bpm", 120.0)));
    currentStep.store(juce::jlimit(0, NUM_STEPS - 1, xml.getIntAttribute("currentStep", 0)));
    sampleCounter.store(0);
    updateStepTiming();

    const char* paramNames[] = { "filter", "pan", "delay", "reverb", "volume" };

    for (int param = 0; param < NumParameters; ++param)
    {
        juce::String stepData = xml.getStringAttribute(paramNames[param]);
        juce::StringArray tokens;
        tokens.addTokens(stepData, ",", "");

        for (int step = 0; step < NUM_STEPS && step < tokens.size(); ++step)
            steps[param][step].store(juce::jlimit(0.0f, 1.0f, tokens[step].getFloatValue()));
    }
}
