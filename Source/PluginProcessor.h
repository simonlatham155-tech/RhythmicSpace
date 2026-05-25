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
    // Parameter access
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    const juce::AudioProcessorValueTreeState& getParameters() const { return parameters; }
    
    // Step sequencer access
    StepSequencer& getStepSequencer() { return stepSequencer; }
    const StepSequencer& getStepSequencer() const { return stepSequencer; }
    
    // Transport control
    void setPlaying(bool shouldPlay);
    bool isPlaying() const { return playing; }
    void setBPM(double newBPM);
    double getBPM() const { return bpm; }
    void setHostSyncEnabled(bool enabled);
    bool isHostSyncEnabled() const { return hostSyncEnabled; }
    
    // Current step for UI visualization
    int getCurrentStep() const { return stepSequencer.getCurrentStep(); }
    
    // Level metering
    float getInputLevel() const { return inputLevel.load(); }
    float getOutputLevel() const { return outputLevel.load(); }
    
    // MIDI activity for UI indicator
    bool hasMIDIActivity() const { return midiActivityFlag.load(); }
    void clearMIDIActivity() { midiActivityFlag.store(false); }
    
    // Preset management
    PresetManager& getPresetManager() { return presetManager; }
    void loadPreset(int presetIndex);
    
    // MIDI controller mapping
    MIDIControllerMap& getMIDIControllerMap() { return midiControllerMap; }  // MIDI enabled! 🎛️

private:
    //==============================================================================
    // Audio Processing Components
    StepSequencer stepSequencer;
    FilterProcessor filterProcessor;
    DelayProcessor delayProcessor;
    ReverbProcessor reverbProcessor;
    PanProcessor panProcessor;
    VolumeProcessor volumeProcessor;
    
    // Preset system
    PresetManager presetManager;
    
    // MIDI controller mapping
    MIDIControllerMap midiControllerMap;  // MIDI enabled! 🎛️
    
    // Parameters
    juce::AudioProcessorValueTreeState parameters;
    
    // Transport state
    std::atomic<bool> playing { false };
    std::atomic<double> bpm { 120.0 };
    std::atomic<bool> hostSyncEnabled { false };
    
    // Level meters
    std::atomic<float> inputLevel { 0.0f };
    std::atomic<float> outputLevel { 0.0f };
    
    // MIDI activity flag
    std::atomic<bool> midiActivityFlag { false };
    
    // Sample rate
    double currentSampleRate = 44100.0;
    
    // Create parameter layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmicSpaceAudioProcessor)
};