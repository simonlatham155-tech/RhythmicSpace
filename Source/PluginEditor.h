#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/MetallicLookAndFeel.h"
#include "UI/StepSequencerComponent.h"
#include "UI/TransportComponent.h"
#include "UI/KnobComponent.h"
#include "UI/FaderComponent.h"
#include "UI/LevelMeterComponent.h"
#include "UI/PresetBrowserComponent.h"
#include "UI/EffectPanelComponent.h"
#include "UI/MIDILearnComponent.h"

//==============================================================================
/**
 * Main plugin editor window
 * 
 * NOW WITH 5 UNIQUE COLORED STEP SEQUENCER LANES! 🎨
 */
class RhythmicSpaceAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            public juce::Timer
{
public:
    RhythmicSpaceAudioProcessorEditor (RhythmicSpaceAudioProcessor&);
    ~RhythmicSpaceAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    
    // Mouse handling (disabled for plugin - UI is locked in place)
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    
private:
    RhythmicSpaceAudioProcessor& audioProcessor;
    
    // Custom look and feel
    MetallicLookAndFeel metallicLookAndFeel;
    
    // UI Components - 5 SEPARATE COLORED STEP SEQUENCERS! ✅
    std::unique_ptr<StepSequencerComponent> filterStepSequencer;   // 🟠 Orange
    std::unique_ptr<StepSequencerComponent> panStepSequencer;      // 🔵 Cyan
    std::unique_ptr<StepSequencerComponent> delayStepSequencer;    // 🟢 Green
    std::unique_ptr<StepSequencerComponent> reverbStepSequencer;   // 🟣 Magenta
    std::unique_ptr<StepSequencerComponent> volumeStepSequencer;   // 🟡 Amber
    
    std::unique_ptr<TransportComponent> transport;
    std::unique_ptr<EffectPanelComponent> effectPanel;
    std::unique_ptr<PresetBrowserComponent> presetBrowser;
    std::unique_ptr<MIDILearnComponent> midiLearn;  // MIDI Learn enabled! 🎛️
    std::unique_ptr<LevelMeterComponent> inputMeter;
    std::unique_ptr<LevelMeterComponent> outputMeter;
    std::unique_ptr<KnobComponent> masterVolumeKnob;
    
    // Window manipulation components
    std::unique_ptr<juce::ResizableCornerComponent> resizableCorner;
    juce::ComponentBoundsConstrainer resizeLimits;
    juce::ComponentDragger windowDragger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmicSpaceAudioProcessorEditor)
};