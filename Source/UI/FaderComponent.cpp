#include "FaderComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
FaderComponent::FaderComponent(const juce::String& labelText,
                               juce::AudioProcessorValueTreeState& vts,
                               const juce::String& parameterID)
{
    // Fader
    fader.setSliderStyle(juce::Slider::LinearVertical);
    fader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(fader);
    
    // Label
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_TERTIARY);
    addAndMakeVisible(label);
    
    // Attach to parameter
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, parameterID, fader);
}

FaderComponent::~FaderComponent()
{
}

//==============================================================================
void FaderComponent::paint(juce::Graphics& g)
{
}

void FaderComponent::resized()
{
    auto area = getLocalBounds();
    label.setBounds(area.removeFromBottom(20));
    fader.setBounds(area);
}
