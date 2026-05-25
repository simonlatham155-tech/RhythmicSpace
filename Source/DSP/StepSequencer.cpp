#include "StepSequencer.h"

//==============================================================================
StepSequencer::StepSequencer()
{
    // Initialize all steps to 0.5 (center)
    for (auto& paramSteps : steps)
        paramSteps.fill(0.5f);
}

StepSequencer::~StepSequencer()
{
}

//==============================================================================
void StepSequencer::prepare(double newSampleRate, double newBPM)
{
    sampleRate = newSampleRate;
    bpm = newBPM;
    updateStepTiming();
    reset();
}

void StepSequencer::updateStepTiming()
{
    // Calculate samples per 16th note
    double beatsPerSecond = bpm / 60.0;
    double sixteenthNotesPerSecond = beatsPerSecond * 4.0; // 4 sixteenth notes per beat
    double samplesPerSixteenthNote = sampleRate / sixteenthNotesPerSecond;
    
    samplesPerStep = static_cast<int>(samplesPerSixteenthNote);
}

void StepSequencer::process(int numSamples)
{
    sampleCounter += numSamples;
    
    // Advance to next step if needed
    while (sampleCounter >= samplesPerStep)
    {
        sampleCounter -= samplesPerStep;
        currentStep = (currentStep + 1) % NUM_STEPS;
    }
}

void StepSequencer::reset()
{
    currentStep = 0;
    sampleCounter = 0;
}

//==============================================================================
void StepSequencer::setBPM(double newBPM)
{
    bpm = juce::jlimit(60.0, 240.0, newBPM);
    updateStepTiming();
}

void StepSequencer::setStepValue(int parameter, int step, float value)
{
    if (parameter >= 0 && parameter < NumParameters && step >= 0 && step < NUM_STEPS)
    {
        steps[parameter][step] = juce::jlimit(0.0f, 1.0f, value);
    }
}

float StepSequencer::getStepValue(int parameter, int step) const
{
    if (parameter >= 0 && parameter < NumParameters && step >= 0 && step < NUM_STEPS)
        return steps[parameter][step];
    return 0.5f;
}

std::array<float, 16> StepSequencer::getSteps(int parameter) const
{
    if (parameter >= 0 && parameter < NumParameters)
        return steps[parameter];
    
    // Return default array with all 0.5f
    std::array<float, 16> defaultSteps;
    defaultSteps.fill(0.5f);
    return defaultSteps;
}

ModulationValues StepSequencer::getCurrentModulationValues() const
{
    ModulationValues values;
    values.filter = steps[FilterParam][currentStep];
    values.pan = steps[PanParam][currentStep];
    values.delay = steps[DelayParam][currentStep];
    values.reverb = steps[ReverbParam][currentStep];
    values.volume = steps[VolumeParam][currentStep];
    return values;
}

//==============================================================================
void StepSequencer::randomizeParameter(int parameter)
{
    if (parameter >= 0 && parameter < NumParameters)
    {
        juce::Random random;
        for (auto& step : steps[parameter])
            step = random.nextFloat();
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
        steps[parameter].fill(0.5f);
}

void StepSequencer::clearAll()
{
    for (auto& paramSteps : steps)
        paramSteps.fill(0.5f);
}

//==============================================================================
void StepSequencer::loadPreset(const Preset& preset)
{
    steps[FilterParam] = preset.filterSteps;
    steps[PanParam] = preset.panSteps;
    steps[DelayParam] = preset.delaySteps;
    steps[ReverbParam] = preset.reverbSteps;
    steps[VolumeParam] = preset.volumeSteps;
    
    setBPM(preset.bpm);
}

void StepSequencer::savePreset(Preset& preset) const
{
    preset.filterSteps = steps[FilterParam];
    preset.panSteps = steps[PanParam];
    preset.delaySteps = steps[DelayParam];
    preset.reverbSteps = steps[ReverbParam];
    preset.volumeSteps = steps[VolumeParam];
    preset.bpm = bpm;
}

//==============================================================================
void StepSequencer::saveState(juce::XmlElement& xml) const
{
    xml.setAttribute("bpm", bpm);
    xml.setAttribute("currentStep", currentStep);
    
    // Save each parameter's steps
    const char* paramNames[] = {"filter", "pan", "delay", "reverb", "volume"};
    
    for (int param = 0; param < NumParameters; ++param)
    {
        juce::String stepData;
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            if (step > 0) stepData += ",";
            stepData += juce::String(steps[param][step], 4);
        }
        xml.setAttribute(paramNames[param], stepData);
    }
}

void StepSequencer::loadState(const juce::XmlElement& xml)
{
    bpm = xml.getDoubleAttribute("bpm", 120.0);
    currentStep = xml.getIntAttribute("currentStep", 0);
    updateStepTiming();
    
    // Load each parameter's steps
    const char* paramNames[] = {"filter", "pan", "delay", "reverb", "volume"};
    
    for (int param = 0; param < NumParameters; ++param)
    {
        juce::String stepData = xml.getStringAttribute(paramNames[param]);
        juce::StringArray tokens;
        tokens.addTokens(stepData, ",", "");
        
        for (int step = 0; step < NUM_STEPS && step < tokens.size(); ++step)
        {
            steps[param][step] = tokens[step].getFloatValue();
        }
    }
}
