#pragma once

#include <JuceHeader.h>
#include "DSP/StepSequencer.h"
#include "DSP/FilterProcessor.h"
#include "DSP/DelayProcessor.h"
#include "DSP/ReverbProcessor.h"
#include "DSP/PanProcessor.h"
#include "DSP/VolumeProcessor.h"
#include "Data/PresetManager.h"
#include "Data/MIDIControllerMap.h"

//==============================================================================
/**
 * RhythmicSpace Audio Processor
 * Step sequencer-based multi-effect processor with rhythmic modulation
 */
class RhythmicSpaceAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RhythmicSpaceAudioProcessor();
    ~RhythmicSpaceAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    const juce::AudioProcessorValueTreeState& getParameters() const { return parameters; }
    
    StepSequencer& getStepSequencer() { return stepSequencer; }
    const StepSequencer& getStepSequencer() const { return stepSequencer; }
    
    void setPlaying(bool shouldPlay);
    bool isPlaying() const { return playing; }
    void setBPM(double newBPM);
    double getBPM() const { return bpm; }
    void setHostSyncEnabled(bool enabled);
    bool isHostSyncEnabled() const { return hostSyncEnabled; }
    
    int getCurrentStep() const { return stepSequencer.getCurrentStep(); }
    
    float getInputLevel() const { return inputLevel.load(); }
    float getOutputLevel() const { return outputLevel.load(); }
    
    bool hasMIDIActivity() const { return midiActivityFlag.load(); }
    void clearMIDIActivity() { midiActivityFlag.store(false); }
    
    PresetManager& getPresetManager() { return presetManager; }
    void loadPreset(int presetIndex);
    
    MIDIControllerMap& getMIDIControllerMap() { return midiControllerMap; }

private:
    StepSequencer stepSequencer;
    FilterProcessor filterProcessor;
    DelayProcessor delayProcessor;
    ReverbProcessor reverbProcessor;
    PanProcessor panProcessor;
    VolumeProcessor volumeProcessor;
    
    PresetManager presetManager;
    MIDIControllerMap midiControllerMap;
    juce::AudioProcessorValueTreeState parameters;
    
    std::atomic<bool> playing { false };
    std::atomic<double> bpm { 120.0 };
    std::atomic<bool> hostSyncEnabled { false };
    
    std::atomic<float> inputLevel { 0.0f };
    std::atomic<float> outputLevel { 0.0f };
    std::atomic<bool> midiActivityFlag { false };
    
    int currentProgramIndex = 0;
    double currentSampleRate = 44100.0;
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void syncRuntimeStateFromParameters();
    void applyPresetParameters(const Preset& preset);
    int getFilterTypeIndex() const;
    void processEffectChain(juce::dsp::ProcessContextReplacing<float>& context, const ModulationValues& modValues);
    void updateHostTransportState();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmicSpaceAudioProcessor)
};
