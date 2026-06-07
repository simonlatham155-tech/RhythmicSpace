#pragma once

#include <JuceHeader.h>
#include "../Data/PresetManager.h"
#include "../PluginProcessor.h"

//==============================================================================
class PresetBrowserComponent : public juce::Component,
                                private juce::ComboBox::Listener,
                                private juce::Button::Listener
{
public:
    PresetBrowserComponent(PresetManager& pm, RhythmicSpaceAudioProcessor& proc);
    ~PresetBrowserComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void refreshPresetList(int selectedIndex = -1, juce::NotificationType notification = juce::sendNotification);

private:
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void buttonClicked(juce::Button* button) override;
    void showSavePresetDialog();
    
    PresetManager& presetManager;
    RhythmicSpaceAudioProcessor& processor;
    
    juce::ComboBox presetCombo;
    juce::Label presetLabel;
    juce::TextButton saveButton;
    juce::TextButton deleteButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBrowserComponent)
};
