#include "PresetBrowserComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
PresetBrowserComponent::PresetBrowserComponent(PresetManager& pm, 
                                               RhythmicSpaceAudioProcessor& proc)
    : presetManager(pm), processor(proc)
{
    // Category removed to save space - showing all presets
    // categoryLabel.setText("CATEGORY", juce::dontSendNotification);
    // categoryLabel.setJustificationType(juce::Justification::centredLeft);
    // categoryLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY);
    // addAndMakeVisible(categoryLabel);
    
    // categoryCombo.addItem("All", 1);
    // auto categories = presetManager.getCategories();
    // for (int i = 0; i < categories.size(); ++i)
    //     categoryCombo.addItem(categories[i], i + 2);
    // categoryCombo.setSelectedId(1);
    // categoryCombo.addListener(this);
    // addAndMakeVisible(categoryCombo);
    
    // Preset label
    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setJustificationType(juce::Justification::centredLeft);
    presetLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY);
    addAndMakeVisible(presetLabel);
    
    // Preset combo
    presetCombo.addListener(this);
    addAndMakeVisible(presetCombo);
    
    // Save button
    saveButton.setButtonText("SAVE");
    saveButton.addListener(this);
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
    saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(saveButton);
    
    // Delete button
    deleteButton.setButtonText("DELETE");
    deleteButton.addListener(this);
    deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
    deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(deleteButton);
    
    updatePresetList();
}

PresetBrowserComponent::~PresetBrowserComponent()
{
    // categoryCombo.removeListener(this);
    presetCombo.removeListener(this);
    saveButton.removeListener(this);
    deleteButton.removeListener(this);
}

//==============================================================================
void PresetBrowserComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    MetallicLookAndFeel::drawRecessedPanel(g, bounds);
    
    // Title removed to save space
    // auto titleArea = bounds.removeFromTop(40);
    // MetallicLookAndFeel::drawEmbossedText(g, "PRESETS", titleArea,
    //                                       juce::Justification::centred);
    
    // Debug: Draw button area outline (temporary for debugging)
    #if JUCE_DEBUG
    g.setColour(juce::Colours::red);
    g.drawRect(saveButton.getBounds(), 2);
    g.setColour(juce::Colours::green);
    g.drawRect(deleteButton.getBounds(), 2);
    #endif
}

void PresetBrowserComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Debug: Log component size
    DBG("PresetBrowserComponent size: " << getWidth() << "x" << getHeight());
    
    // Title space removed - start directly with preset controls
    // area.removeFromTop(35); 
    
    // Category section removed to save space
    // categoryLabel.setBounds(area.removeFromTop(20));
    // area.removeFromTop(5);
    // categoryCombo.setBounds(area.removeFromTop(30));
    
    area.removeFromTop(8);  // Minimal top margin
    presetLabel.setBounds(area.removeFromTop(18));
    area.removeFromTop(4);
    presetCombo.setBounds(area.removeFromTop(28));
    
    area.removeFromTop(12); // Space before buttons
    auto buttonArea = area.removeFromTop(28);
    
    // Split buttons evenly
    auto saveButtonBounds = buttonArea.removeFromLeft(buttonArea.getWidth() / 2 - 5);
    DBG("Save button bounds: " << saveButtonBounds.toString());
    saveButton.setBounds(saveButtonBounds);
    
    buttonArea.removeFromLeft(10); // Gap between buttons
    DBG("Delete button bounds: " << buttonArea.toString());
    deleteButton.setBounds(buttonArea);
    
    DBG("Save button visible: " << (saveButton.isVisible() ? "YES" : "NO"));
    DBG("Delete button visible: " << (deleteButton.isVisible() ? "YES" : "NO"));
}

//==============================================================================
void PresetBrowserComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Category filtering removed - only handle preset changes now
    if (comboBox == &presetCombo)
    {
        int presetIndex = presetCombo.getSelectedId() - 1;
        if (presetIndex >= 0)
            processor.loadPreset(presetIndex);
    }
}

void PresetBrowserComponent::updatePresetList()
{
    presetCombo.clear();
    
    // Show all presets (category filtering removed)
    for (int i = 0; i < presetManager.getNumPresets(); ++i)
    {
        if (auto* preset = presetManager.getPreset(i))
            presetCombo.addItem(preset->name, i + 1);
    }
    
    if (presetCombo.getNumItems() > 0)
        presetCombo.setSelectedId(1);
}

void PresetBrowserComponent::buttonClicked(juce::Button* button)
{
    if (button == &saveButton)
    {
        showSavePresetDialog();
    }
    else if (button == &deleteButton)
    {
        int presetIndex = presetCombo.getSelectedId() - 1;
        if (presetIndex >= 0)
        {
            auto* preset = presetManager.getPreset(presetIndex);
            if (preset && preset->category == "User")
            {
                juce::AlertWindow::showOkCancelBox(
                    juce::AlertWindow::QuestionIcon,
                    "Delete Preset",
                    "Are you sure you want to delete '" + preset->name + "'?",
                    "Delete", "Cancel",
                    nullptr,
                    juce::ModalCallbackFunction::create([this, presetIndex](int result) {
                        if (result == 1) // OK button
                        {
                            presetManager.deleteUserPreset(presetIndex);
                            updatePresetList();
                        }
                    })
                );
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
                // Create preset from current state
                auto preset = presetManager.createPresetFromCurrentState(
                    presetName,
                    category,
                    processor.getStepSequencer(),
                    processor.getParameters()
                );
                
                // Save it
                presetManager.saveUserPreset(preset);
                
                // Update UI
                updatePresetList();
                
                // Show confirmation
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