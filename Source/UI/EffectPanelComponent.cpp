#include "EffectPanelComponent.h"
#include "MetallicLookAndFeel.h"

//==============================================================================
// Helper class for panel layout with proper component ownership
class EffectPanel : public juce::Component
{
public:
    EffectPanel() = default;
    
    ~EffectPanel() override
    {
        // OwnedArray will automatically delete all child components
        childComponents.clear();
    }
    
    void resized() override
    {
        if (layoutFunction)
            layoutFunction();
    }
    
    // Add a component with proper ownership
    template<typename ComponentType>
    ComponentType* addOwnedComponent(std::unique_ptr<ComponentType> component)
    {
        auto* ptr = component.get();
        childComponents.add(component.release());
        addAndMakeVisible(ptr);
        return ptr;
    }
    
    // Track KnobComponents for MIDI learn
    void trackKnob(KnobComponent* knob)
    {
        knobs.push_back(knob);
    }
    
    std::vector<KnobComponent*>& getKnobs() { return knobs; }
    
    std::function<void()> layoutFunction;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    
private:
    juce::OwnedArray<juce::Component> childComponents;
    std::vector<KnobComponent*> knobs;
};

//==============================================================================
EffectPanelComponent::EffectPanelComponent(juce::AudioProcessorValueTreeState& vts)
    : valueTreeState(vts),
      tabbedComponent(juce::TabbedButtonBar::TabsAtTop)
{
    // Add tabs for each effect
    tabbedComponent.addTab("FILTER", MetallicLookAndFeel::RECESS_MID,
                          createFilterPanel().release(), true);
    tabbedComponent.addTab("DELAY", MetallicLookAndFeel::RECESS_MID,
                          createDelayPanel().release(), true);
    tabbedComponent.addTab("REVERB", MetallicLookAndFeel::RECESS_MID,
                          createReverbPanel().release(), true);
    tabbedComponent.addTab("PAN", MetallicLookAndFeel::RECESS_MID,
                          createPanPanel().release(), true);
    tabbedComponent.addTab("VOLUME", MetallicLookAndFeel::RECESS_MID,
                          createVolumePanel().release(), true);
    
    addAndMakeVisible(tabbedComponent);
}

EffectPanelComponent::~EffectPanelComponent()
{
}

//==============================================================================
void EffectPanelComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    MetallicLookAndFeel::drawRecessedPanel(g, bounds);
}

void EffectPanelComponent::resized()
{
    tabbedComponent.setBounds(getLocalBounds().reduced(5));
}

//==============================================================================
void EffectPanelComponent::setMIDILearnCallback(std::function<void(const juce::String&)> callback)
{
    // Apply callback to all knobs in all tabs
    for (int i = 0; i < tabbedComponent.getNumTabs(); ++i)
    {
        auto* tabContent = tabbedComponent.getTabContentComponent(i);
        if (auto* effectPanel = dynamic_cast<EffectPanel*>(tabContent))
        {
            for (auto* knob : effectPanel->getKnobs())
            {
                knob->onMIDILearnRequest = callback;
                allKnobs.push_back(knob);
            }
        }
    }
}

//==============================================================================
std::unique_ptr<juce::Component> EffectPanelComponent::createFilterPanel()
{
    auto panel = std::make_unique<EffectPanel>();
    
    // Create components with proper ownership
    auto* cutoffKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("CUTOFF", valueTreeState, "filterCutoff"));
    panel->trackKnob(cutoffKnob);
    
    auto* resKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("RESONANCE", valueTreeState, "filterResonance"));
    panel->trackKnob(resKnob);
    
    auto* mixKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("MIX", valueTreeState, "filterMix"));
    panel->trackKnob(mixKnob);
    
    auto typeCombo = std::make_unique<juce::ComboBox>();
    typeCombo->addItem("Low Pass", 1);
    typeCombo->addItem("High Pass", 2);
    typeCombo->addItem("Band Pass", 3);
    auto* typeComboPtr = panel->addOwnedComponent(std::move(typeCombo));
    panel->filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        valueTreeState, "filterType", *typeComboPtr);
    
    panel->setSize(400, 120);
    
    // Layout lambda - capture raw pointers (safe because panel owns them)
    panel->layoutFunction = [cutoffKnob, resKnob, mixKnob, typeComboPtr]()
    {
        auto area = juce::Rectangle<int>(0, 0, 400, 120).reduced(10);
        auto row = area.removeFromTop(100);
        cutoffKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        resKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        mixKnob->setBounds(row.removeFromLeft(100));
        
        area.removeFromTop(10);
        typeComboPtr->setBounds(area.removeFromTop(30).removeFromLeft(150));
    };
    
    return panel;
}

std::unique_ptr<juce::Component> EffectPanelComponent::createDelayPanel()
{
    auto panel = std::make_unique<EffectPanel>();
    
    auto* timeKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("TIME", valueTreeState, "delayTime"));
    panel->trackKnob(timeKnob);
    
    auto* feedbackKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("FEEDBACK", valueTreeState, "delayFeedback"));
    panel->trackKnob(feedbackKnob);
    
    auto* mixKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("MIX", valueTreeState, "delayMix"));
    panel->trackKnob(mixKnob);
    
    panel->setSize(400, 120);
    
    panel->layoutFunction = [timeKnob, feedbackKnob, mixKnob]()
    {
        auto area = juce::Rectangle<int>(0, 0, 400, 120).reduced(10);
        auto row = area.removeFromTop(100);
        timeKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        feedbackKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        mixKnob->setBounds(row.removeFromLeft(100));
    };
    
    return panel;
}

std::unique_ptr<juce::Component> EffectPanelComponent::createReverbPanel()
{
    auto panel = std::make_unique<EffectPanel>();
    
    auto* sizeKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("SIZE", valueTreeState, "reverbSize"));
    panel->trackKnob(sizeKnob);
    
    auto* dampingKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("DAMPING", valueTreeState, "reverbDamping"));
    panel->trackKnob(dampingKnob);
    
    auto* mixKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("MIX", valueTreeState, "reverbMix"));
    panel->trackKnob(mixKnob);
    
    panel->setSize(400, 120);
    
    panel->layoutFunction = [sizeKnob, dampingKnob, mixKnob]()
    {
        auto area = juce::Rectangle<int>(0, 0, 400, 120).reduced(10);
        auto row = area.removeFromTop(100);
        sizeKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        dampingKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        mixKnob->setBounds(row.removeFromLeft(100));
    };
    
    return panel;
}

std::unique_ptr<juce::Component> EffectPanelComponent::createPanPanel()
{
    auto panel = std::make_unique<EffectPanel>();
    
    auto* widthKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("WIDTH", valueTreeState, "panWidth"));
    panel->trackKnob(widthKnob);
    
    auto* rateKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("RATE", valueTreeState, "panRate"));
    panel->trackKnob(rateKnob);
    
    panel->setSize(400, 120);
    
    panel->layoutFunction = [widthKnob, rateKnob]()
    {
        auto area = juce::Rectangle<int>(0, 0, 400, 120).reduced(10);
        auto row = area.removeFromTop(100);
        widthKnob->setBounds(row.removeFromLeft(100));
        row.removeFromLeft(10);
        rateKnob->setBounds(row.removeFromLeft(100));
    };
    
    return panel;
}

std::unique_ptr<juce::Component> EffectPanelComponent::createVolumePanel()
{
    auto panel = std::make_unique<EffectPanel>();
    
    auto* amountKnob = panel->addOwnedComponent(
        std::make_unique<KnobComponent>("AMOUNT", valueTreeState, "volumeAmount"));
    panel->trackKnob(amountKnob);
    
    panel->setSize(400, 120);
    
    panel->layoutFunction = [amountKnob]()
    {
        auto area = juce::Rectangle<int>(0, 0, 400, 120).reduced(10);
        auto row = area.removeFromTop(100);
        amountKnob->setBounds(row.removeFromLeft(100));
    };
    
    return panel;
}