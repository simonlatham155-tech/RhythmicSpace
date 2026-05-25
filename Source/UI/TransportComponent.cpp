#include "TransportComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
TransportComponent::TransportComponent(RhythmicSpaceAudioProcessor& proc)
    : processor(proc)
{
    // Play button
    playButton.setButtonText("PLAY");
    playButton.setClickingTogglesState(true);
    playButton.addListener(this);
    addAndMakeVisible(playButton);
    
    // Stop button
    stopButton.setButtonText("STOP");
    stopButton.addListener(this);
    addAndMakeVisible(stopButton);
    
    // Randomize button (accent style)
    randomizeButton.setButtonText("RANDOMIZE");
    randomizeButton.setName("accent");  // Triggers accent button styling
    randomizeButton.addListener(this);
    addAndMakeVisible(randomizeButton);
    
    // Sync button
    syncButton.setButtonText("SYNC");
    syncButton.setClickingTogglesState(true);
    syncButton.setToggleState(false, juce::dontSendNotification);
    syncButton.addListener(this);
    addAndMakeVisible(syncButton);
    
    // BPM slider
    bpmSlider.setRange(60.0, 240.0, 1.0);
    bpmSlider.setValue(120.0);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 25);
    bpmSlider.addListener(this);
    addAndMakeVisible(bpmSlider);
    
    // BPM label
    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.setJustificationType(juce::Justification::centredRight);
    bpmLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY);
    addAndMakeVisible(bpmLabel);
    
    // Initial state
    updateBpmControlState();
}

TransportComponent::~TransportComponent()
{
    playButton.removeListener(this);
    stopButton.removeListener(this);
    randomizeButton.removeListener(this);
    syncButton.removeListener(this);
    bpmSlider.removeListener(this);
}

//==============================================================================
void TransportComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    MetallicLookAndFeel::drawHeader(g, bounds);
}

void TransportComponent::resized()
{
    auto area = getLocalBounds().reduced(5);
    
    auto buttonArea = area.removeFromLeft(350);
    playButton.setBounds(buttonArea.removeFromLeft(80).reduced(2));
    buttonArea.removeFromLeft(5);
    stopButton.setBounds(buttonArea.removeFromLeft(80).reduced(2));
    buttonArea.removeFromLeft(10);
    randomizeButton.setBounds(buttonArea.removeFromLeft(120).reduced(2));
    
    area.removeFromLeft(20);
    syncButton.setBounds(area.removeFromLeft(70).reduced(2));
    area.removeFromLeft(10);
    bpmLabel.setBounds(area.removeFromLeft(50));
    area.removeFromLeft(5);
    bpmSlider.setBounds(area.removeFromLeft(200));
}

//==============================================================================
void TransportComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        processor.setPlaying(playButton.getToggleState());
    }
    else if (button == &stopButton)
    {
        processor.setPlaying(false);
        playButton.setToggleState(false, juce::dontSendNotification);
        processor.getStepSequencer().reset();
    }
    else if (button == &randomizeButton)
    {
        processor.getStepSequencer().randomizeAll();
    }
    else if (button == &syncButton)
    {
        processor.setHostSyncEnabled(syncButton.getToggleState());
        updateBpmControlState();
    }
}

void TransportComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &bpmSlider)
    {
        processor.setBPM(bpmSlider.getValue());
    }
}

void TransportComponent::updateBpmControlState()
{
    bool isSynced = syncButton.getToggleState();
    bpmSlider.setEnabled(!isSynced);
    bpmLabel.setEnabled(!isSynced);
    
    // Update visual appearance
    if (isSynced)
    {
        bpmLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY.withAlpha(0.5f));
    }
    else
    {
        bpmLabel.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_SECONDARY);
    }
}
