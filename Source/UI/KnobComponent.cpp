#include "KnobComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
KnobComponent::KnobComponent(const juce::String& labelText,
                             juce::AudioProcessorValueTreeState& vts,
                             const juce::String& parameterID)
    : paramID(parameterID)
{
    // Knob
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    
    // For MASTER knob, put value above the knob for tighter layout
    if (labelText == "MASTER")
        knob.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 50, 16);
    else
        knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    knob.addListener(this);
    
    // Override mouse events on the slider to intercept right-clicks
    knob.addMouseListener(this, true);
    
    addAndMakeVisible(knob);
    
    // Label
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, MetallicLookAndFeel::TEXT_TERTIARY);
    addAndMakeVisible(label);
    
    // Attach to parameter
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        vts, parameterID, knob);
}

KnobComponent::~KnobComponent()
{
    knob.removeListener(this);
    knob.removeMouseListener(this);
}

//==============================================================================
void KnobComponent::paint(juce::Graphics& g)
{
}

void KnobComponent::resized()
{
    auto area = getLocalBounds();
    
    // For MASTER knob, use tighter spacing
    if (label.getText() == "MASTER")
    {
        label.setBounds(area.removeFromTop(18));
        area.removeFromTop(2); // Minimal gap
        knob.setBounds(area);
    }
    else
    {
        // Standard spacing for other knobs
        label.setBounds(area.removeFromTop(20));
        area.removeFromTop(5);
        knob.setBounds(area);
    }
}

void KnobComponent::mouseDown(const juce::MouseEvent& e)
{
    // Right-click or Ctrl+Click to enter MIDI learn mode
    if (e.mods.isPopupMenu() || e.mods.isCommandDown())
    {
        if (onMIDILearnRequest)
        {
            onMIDILearnRequest(paramID);
            
            // Show popup notification
            juce::PopupMenu menu;
            menu.addItem(1, "MIDI Learn: " + label.getText(), false);
            menu.addSeparator();
            menu.addItem(2, "Move a MIDI controller to assign...", false);
            menu.showMenuAsync(juce::PopupMenu::Options());
        }
    }
    else
    {
        // Forward normal clicks to the knob
        Component::mouseDown(e);
    }
}