#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * Modulation values for each effect parameter
 */
struct ModulationValues
{
    float filter = 0.5f;   // 0.0 - 1.0
    float pan = 0.5f;
    float delay = 0.5f;
    float reverb = 0.5f;
    float volume = 0.5f;
};

//==============================================================================
/**
 * Preset data structure
 */
struct Preset
{
    juce::String name;
    juce::String category;
    double bpm;
    
    // Effect parameters
    float filterCutoff = 1000.0f;
    float filterResonance = 0.7f;
    int filterType = 0;
    float filterMix = 100.0f;
    float delayTime = 0.5f;
    float delayFeedback = 0.5f;
    float delayMix = 50.0f;
    float reverbSize = 0.5f;
    float reverbDamping = 0.5f;
    float reverbMix = 30.0f;
    
    // Step data for each parameter (16 steps, 0.0-1.0)
    std::array<float, 16> filterSteps;
    std::array<float, 16> panSteps;
    std::array<float, 16> delaySteps;
    std::array<float, 16> reverbSteps;
    std::array<float, 16> volumeSteps;
};

//==============================================================================
/**
 * 16-step sequencer with tempo sync
 */
class StepSequencer
{
public:
    StepSequencer();
    ~StepSequencer();
    
    // Initialization
    void prepare(double sampleRate, double bpm);
    
    // Processing
    void process(int numSamples);
    void reset();
    
    // Transport
    void setBPM(double newBPM);
    double getBPM() const { return bpm; }
    
    // Step access
    void setStepValue(int parameter, int step, float value);
    float getStepValue(int parameter, int step) const;
    std::array<float, 16> getSteps(int parameter) const;
    int getCurrentStep() const { return currentStep; }
    
    // Modulation output
    ModulationValues getCurrentModulationValues() const;
    
    // Randomization
    void randomizeParameter(int parameter);
    void randomizeAll();
    void clearParameter(int parameter);
    void clearAll();
    
    // Preset management
    void loadPreset(const Preset& preset);
    void savePreset(Preset& preset) const;
    
    // State save/load
    void saveState(juce::XmlElement& xml) const;
    void loadState(const juce::XmlElement& xml);
    
    // Parameter indices
    enum Parameter
    {
        FilterParam = 0,
        PanParam,
        DelayParam,
        ReverbParam,
        VolumeParam,
        NumParameters
    };

private:
    static constexpr int NUM_STEPS = 16;
    
    // Step data
    std::array<std::array<float, NUM_STEPS>, NumParameters> steps;
    
    // Timing
    double sampleRate = 44100.0;
    double bpm = 120.0;
    int samplesPerStep = 0;
    int sampleCounter = 0;
    int currentStep = 0;
    
    // Calculate samples per step based on BPM
    void updateStepTiming();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepSequencer)
};
