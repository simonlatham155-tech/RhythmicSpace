#include "PresetBrowserComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
PresetBrowserComponent::PresetBrowserComponent(PresetManager& pm, 
                                               RhythmicSpaceAudioProcessor& proc)
    : presetManager(pm), processor(proc)
{
    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setJustificationType(juce::Justification::centredLeft);
    presetLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY);
    addAndMakeVisible(presetLabel);
    
    presetCombo.addListener(this);
    addAndMakeVisible(presetCombo);
    
    saveButton.setButtonText("SAVE");
    saveButton.addListener(this);
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
    saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(saveButton);
    
    deleteButton.setButtonText("DELETE");
    deleteButton.addListener(this);
    deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
    deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(deleteButton);
    
    refreshPresetList(processor.getCurrentProgram(), juce::dontSendNotification);
}

PresetBrowserComponent::~PresetBrowserComponent()
{
    presetCombo.removeListener(this);
    saveButton.removeListener(this);
    deleteButton.removeListener(this);
}

//==============================================================================
void PresetBrowserComponent::paint(juce::Graphics& g)
{
    MetallicLookAndFeel::drawRecessedPanel(g, getLocalBounds());
}

void PresetBrowserComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    area.removeFromTop(8);
    presetLabel.setBounds(area.removeFromTop(18));
    area.removeFromTop(4);
    presetCombo.setBounds(area.removeFromTop(28));
    
    area.removeFromTop(12);
    auto buttonArea = area.removeFromTop(28);
    
    saveButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2 - 5));
    buttonArea.removeFromLeft(10);
    deleteButton.setBounds(buttonArea);
}

//==============================================================================
void PresetBrowserComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &presetCombo)
    {
        const int presetIndex = presetCombo.getSelectedId() - 1;
        if (presetIndex >= 0)
            processor.loadPreset(presetIndex);
    }
}

void PresetBrowserComponent::refreshPresetList(int selectedIndex, juce::NotificationType notification)
{
    presetCombo.clear(juce::dontSendNotification);
    
    for (int i = 0; i < presetManager.getNumPresets(); ++i)
    {
        if (auto* preset = presetManager.getPreset(i))
            presetCombo.addItem(preset->name, i + 1);
    }
    
    if (presetCombo.getNumItems() > 0)
    {
        int selectedId = selectedIndex >= 0 ? selectedIndex + 1 : 1;
        selectedId = juce::jlimit(1, presetCombo.getNumItems(), selectedId);
        presetCombo.setSelectedId(selectedId, notification);
    }
}

void PresetBrowserComponent::buttonClicked(juce::Button* button)
{
    if (button == &saveButton)
    {
        showSavePresetDialog();
    }
    else if (button == &deleteButton)
    {
        const int presetIndex = presetCombo.getSelectedId() - 1;

        if (presetIndex >= presetManager.getFactoryPresetCount())
        {
            if (auto* preset = presetManager.getPreset(presetIndex))
            {
                juce::AlertWindow::showOkCancelBox(
                    juce::AlertWindow::QuestionIcon,
                    "Delete Preset",
                    "Are you sure you want to delete '" + preset->name + "'?",
                    "Delete", "Cancel",
                    nullptr,
                    juce::ModalCallbackFunction::create([this, presetIndex](int result) {
                        if (result == 1)
                        {
                            presetManager.deleteUserPreset(presetIndex);
                            refreshPresetList(juce::jmin(presetIndex, presetManager.getNumPresets() - 1),
                                              juce::dontSendNotification);
                        }
                    })
                );
            }
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Cannot Delete",
                "You can only delete user presets, not factory presets."
            );
        }
    }
}

void PresetBrowserComponent::showSavePresetDialog()
{
    auto* window = new juce::AlertWindow("Save Preset", 
                                         "Enter a name for your preset:",
                                         juce::AlertWindow::QuestionIcon);
    
    window->addTextEditor("name", "My Preset", "Preset Name:");
    window->addComboBox("category", {"User", "Custom", "Experimental"}, "Category:");
    window->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
    
    window->enterModalState(true, juce::ModalCallbackFunction::create([this, window](int result) {
        if (result == 1)
        {
            juce::String presetName = window->getTextEditorContents("name");
            juce::String category = window->getComboBoxComponent("category")->getText();
            
            if (presetName.isNotEmpty())
            {
                auto preset = presetManager.createPresetFromCurrentState(
                    presetName,
                    category,
                    processor.getStepSequencer(),
                    processor.getParameters()
                );
                
                presetManager.saveUserPreset(preset);
                
                int savedIndex = -1;
                for (int i = 0; i < presetManager.getNumPresets(); ++i)
                {
                    if (auto* savedPreset = presetManager.getPreset(i); savedPreset && savedPreset->name == presetName)
                    {
                        savedIndex = i;
                        break;
                    }
                }

                processor.loadPreset(savedIndex);
                refreshPresetList(savedIndex, juce::dontSendNotification);
                
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Preset Saved",
                    "Preset '" + presetName + "' has been saved successfully!"
                );
            }
        }
        delete window;
    }), true);
}
