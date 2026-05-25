#pragma once

#include <JuceHeader.h>
#include "../Data/PresetManager.h"
#include "../PluginProcessor.h"

//==============================================================================
/**
 * Preset browser with category filtering
 */
class PresetBrowserComponent : public juce::Component,
                                private juce::ComboBox::Listener,
                                private juce::Button::Listener
{
public:
    PresetBrowserComponent(PresetManager& pm, RhythmicSpaceAudioProcessor& proc);
    ~PresetBrowserComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;
    void updatePresetList();
    void showSavePresetDialog();
    
    PresetManager& presetManager;
    RhythmicSpaceAudioProcessor& processor;
    
    // Category dropdown removed to save space
    // juce::ComboBox categoryCombo;
    juce::ComboBox presetCombo;
    // juce::Label categoryLabel;
    juce::Label presetLabel;
    juce::TextButton saveButton;
    juce::TextButton deleteButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBrowserComponent)
};
