#include "MIDILearnComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
MIDILearnComponent::MIDILearnComponent(MIDIControllerMap& map)
    : midiMap(map)
{
    // Title label
    titleLabel.setText("MIDI CONTROL", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_PRIMARY);
    addAndMakeVisible(titleLabel);
    
    // Status label
    statusLabel.setText("Right-click any knob to learn", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY);
    addAndMakeVisible(statusLabel);
    
    // Template selector
    templateCombo.addItem("No Template", 1);
    templateCombo.addItem("Arturia MiniLab 3", 2);
    templateCombo.addItem("Custom Mapping", 3);
    templateCombo.setSelectedId(1);
    templateCombo.addListener(this);
    addAndMakeVisible(templateCombo);
    
    // Clear mappings button
    clearButton.setButtonText("CLEAR ALL");
    clearButton.addListener(this);
    clearButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
    clearButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(clearButton);
    
    // Cancel learn button (only shown when learning)
    learnButton.setButtonText("CANCEL LEARN");
    learnButton.addListener(this);
    learnButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffcc3333));
    learnButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    learnButton.setVisible(true); // Always visible now
    addAndMakeVisible(learnButton);
    
    // Start timer for blinking indicator and updating button state
    startTimerHz(4); // 4 Hz = smoother updates
}

MIDILearnComponent::~MIDILearnComponent()
{
    stopTimer();
    templateCombo.removeListener(this);
    clearButton.removeListener(this);
    learnButton.removeListener(this);
}

//==============================================================================
void MIDILearnComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    MetallicLookAndFeel::drawRecessedPanel(g, bounds);
    
    // Draw MIDI activity indicator in top-right corner
    auto activityArea = bounds.removeFromTop(8).removeFromRight(60).reduced(2);
    if (midiActivityCounter > 0)
    {
        g.setColour(juce::Colour(0xff00ff00)); // Green flash
        g.fillRoundedRectangle(activityArea.toFloat(), 2.0f);
        g.setColour(juce::Colours::black);
        g.setFont(10.0f);
        g.drawText("MIDI IN", activityArea, juce::Justification::centred);
    }
    else
    {
        g.setColour(juce::Colour(0xff2a2a2a)); // Dark gray
        g.fillRoundedRectangle(activityArea.toFloat(), 2.0f);
        g.setColour(juce::Colour(0xff666666));
        g.setFont(10.0f);
        g.drawText("MIDI IN", activityArea, juce::Justification::centred);
    }
    
    // Draw MIDI learn indicator if learning
    if (midiMap.isLearning())
    {
        auto indicatorBounds = bounds.removeFromTop(30).reduced(10, 5);
        
        // Blinking red background
        if (blinkState)
        {
            g.setColour(juce::Colour(0xffff3333));
            g.fillRoundedRectangle(indicatorBounds.toFloat(), 4.0f);
        }
        
        // Text
        g.setColour(juce::Colours::white);
        g.setFont(14.0f);
        g.drawText("LEARNING: " + midiMap.getLearningParameterID(), 
                   indicatorBounds, juce::Justification::centred);
    }
}

void MIDILearnComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Skip top area if learning
    if (midiMap.isLearning())
        area.removeFromTop(30);
    
    area.removeFromTop(5);
    
    // Title
    titleLabel.setBounds(area.removeFromTop(20));
    area.removeFromTop(10);
    
    // Template selector
    auto templateRow = area.removeFromTop(30);
    templateCombo.setBounds(templateRow);
    
    area.removeFromTop(10);
    
    // Buttons row
    auto buttonRow = area.removeFromTop(30);
    int buttonWidth = (buttonRow.getWidth() - 10) / 2;
    
    clearButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(10);
    learnButton.setBounds(buttonRow);
    
    area.removeFromTop(10);
    
    // Status
    statusLabel.setBounds(area.removeFromTop(20));
}

//==============================================================================
void MIDILearnComponent::buttonClicked(juce::Button* button)
{
    if (button == &clearButton)
    {
        midiMap.clearAllMappings();
        statusLabel.setText("All mappings cleared", juce::dontSendNotification);
        repaint();
    }
    else if (button == &learnButton)
    {
        // Only allow cancelling if currently learning
        if (midiMap.isLearning())
        {
            midiMap.stopLearning();
            statusLabel.setText("MIDI Learn cancelled", juce::dontSendNotification);
        }
        else
        {
            // Button shows "READY" when not learning - clicking does nothing
            // (right-click knobs to start learning)
            statusLabel.setText("Right-click any knob to learn", juce::dontSendNotification);
        }
        repaint();
    }
}

void MIDILearnComponent::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &templateCombo)
    {
        int selectedId = templateCombo.getSelectedId();
        
        switch (selectedId)
        {
            case 1: // No template
                midiMap.loadTemplate(MIDIControllerMap::ControllerTemplate::None);
                statusLabel.setText("Template cleared", juce::dontSendNotification);
                break;
                
            case 2: // Arturia MiniLab 3
                midiMap.loadTemplate(MIDIControllerMap::ControllerTemplate::ArturiaMiniLab3);
                statusLabel.setText("Arturia MiniLab 3 loaded", juce::dontSendNotification);
                break;
                
            case 3: // Custom
                midiMap.loadTemplate(MIDIControllerMap::ControllerTemplate::CustomMapping);
                statusLabel.setText("Custom mapping mode", juce::dontSendNotification);
                break;
        }
        
        repaint();
    }
}

void MIDILearnComponent::timerCallback()
{
    // Decrement MIDI activity counter
    if (midiActivityCounter > 0)
        midiActivityCounter--;
    
    if (midiMap.isLearning())
    {
        blinkState = !blinkState;
        
        // Update button to red "CANCEL LEARN"
        learnButton.setButtonText("CANCEL LEARN");
        learnButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffcc3333));
        
        repaint();
    }
    else
    {
        // Update button to gray "READY"
        learnButton.setButtonText("READY");
        learnButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
        
        // Repaint for MIDI activity indicator
        if (midiActivityCounter > 0)
            repaint();
    }
}