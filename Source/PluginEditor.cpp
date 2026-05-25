#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RhythmicSpaceAudioProcessorEditor::RhythmicSpaceAudioProcessorEditor (RhythmicSpaceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set look and feel
    setLookAndFeel(&metallicLookAndFeel);
    
    // Create 5 SEPARATE COLORED STEP SEQUENCERS! ✅
    // Each gets its own unique color based on EffectType
    
    filterStepSequencer = std::make_unique<StepSequencerComponent>(
        "FILTER CUTOFF",
        MetallicLookAndFeel::EffectType::Filter  // 🟠 Orange (#ff6b35)
    );
    addAndMakeVisible(filterStepSequencer.get());
    
    panStepSequencer = std::make_unique<StepSequencerComponent>(
        "PAN POSITION",
        MetallicLookAndFeel::EffectType::Pan  // 🔵 Cyan (#00d4ff)
    );
    addAndMakeVisible(panStepSequencer.get());
    
    delayStepSequencer = std::make_unique<StepSequencerComponent>(
        "DELAY AMOUNT",
        MetallicLookAndFeel::EffectType::Delay  // 🟢 Green (#00ff88)
    );
    addAndMakeVisible(delayStepSequencer.get());
    
    reverbStepSequencer = std::make_unique<StepSequencerComponent>(
        "REVERB AMOUNT",
        MetallicLookAndFeel::EffectType::Reverb  // 🟣 Magenta (#ff00ff)
    );
    addAndMakeVisible(reverbStepSequencer.get());
    
    volumeStepSequencer = std::make_unique<StepSequencerComponent>(
        "VOLUME LEVEL",
        MetallicLookAndFeel::EffectType::Volume  // 🟡 Amber (#ffaa00)
    );
    addAndMakeVisible(volumeStepSequencer.get());
    
    // Setup callbacks to sync with step sequencer data
    filterStepSequencer->onStepChanged = [this](int step, float value) {
        audioProcessor.getStepSequencer().setStepValue(StepSequencer::FilterParam, step, value);
    };
    
    panStepSequencer->onStepChanged = [this](int step, float value) {
        audioProcessor.getStepSequencer().setStepValue(StepSequencer::PanParam, step, value);
    };
    
    delayStepSequencer->onStepChanged = [this](int step, float value) {
        audioProcessor.getStepSequencer().setStepValue(StepSequencer::DelayParam, step, value);
    };
    
    reverbStepSequencer->onStepChanged = [this](int step, float value) {
        audioProcessor.getStepSequencer().setStepValue(StepSequencer::ReverbParam, step, value);
    };
    
    volumeStepSequencer->onStepChanged = [this](int step, float value) {
        audioProcessor.getStepSequencer().setStepValue(StepSequencer::VolumeParam, step, value);
    };
    
    // Create other components
    transport = std::make_unique<TransportComponent>(audioProcessor);
    addAndMakeVisible(transport.get());
    
    effectPanel = std::make_unique<EffectPanelComponent>(audioProcessor.getValueTreeState());
    addAndMakeVisible(effectPanel.get());
    
    presetBrowser = std::make_unique<PresetBrowserComponent>(audioProcessor.getPresetManager(), audioProcessor);
    addAndMakeVisible(presetBrowser.get());
    
    // MIDI Learn component - ENABLED! 🎛️
    midiLearn = std::make_unique<MIDILearnComponent>(audioProcessor.getMIDIControllerMap());
    addAndMakeVisible(midiLearn.get());
    
    inputMeter = std::make_unique<LevelMeterComponent>();
    addAndMakeVisible(inputMeter.get());
    
    outputMeter = std::make_unique<LevelMeterComponent>();
    addAndMakeVisible(outputMeter.get());
    
    masterVolumeKnob = std::make_unique<KnobComponent>("MASTER", 
        audioProcessor.getValueTreeState(), "masterVolume");
    addAndMakeVisible(masterVolumeKnob.get());
    
    // Set MIDI learn callback for master volume knob
    masterVolumeKnob->onMIDILearnRequest = [processor = &audioProcessor](const juce::String& paramID) {
        processor->getMIDIControllerMap().startLearning(paramID);
    };
    
    // Set MIDI learn callback for all effect panel knobs  
    effectPanel->setMIDILearnCallback([processor = &audioProcessor](const juce::String& paramID) {
        processor->getMIDIControllerMap().startLearning(paramID);
    });
    
    // Branding will be drawn directly in paint() for custom styling
    // No need for brandingLabel component anymore
    
    // Set window size constraints and resizable corner
    resizeLimits.setMinimumSize(800, 600);
    resizeLimits.setMaximumSize(2400, 1600);
    
    resizableCorner = std::make_unique<juce::ResizableCornerComponent>(this, &resizeLimits);
    addAndMakeVisible(resizableCorner.get());
    
    // Set window size - matching web app dimensions
    setSize (1200, 800);
    
    // Start timer for UI updates
    startTimerHz(30);
}

RhythmicSpaceAudioProcessorEditor::~RhythmicSpaceAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

//==============================================================================
void RhythmicSpaceAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // OUTER METAL CHASSIS - Raised Bezel (with padding)
    auto chassisBounds = bounds.reduced(0);
    MetallicLookAndFeel::drawOuterChassis(g, chassisBounds);
    
    // Corner mounting screws (4 corners)
    MetallicLookAndFeel::drawCornerScrew(g, juce::Point<float>(10.0f, 10.0f), 4.0f);
    MetallicLookAndFeel::drawCornerScrew(g, juce::Point<float>(bounds.getWidth() - 10.0f, 10.0f), 4.0f);
    MetallicLookAndFeel::drawCornerScrew(g, juce::Point<float>(10.0f, bounds.getHeight() - 10.0f), 4.0f);
    MetallicLookAndFeel::drawCornerScrew(g, juce::Point<float>(bounds.getWidth() - 10.0f, bounds.getHeight() - 10.0f), 4.0f);
    
    // INNER RECESSED PANEL
    auto innerBounds = chassisBounds.reduced(12);
    MetallicLookAndFeel::drawInnerRecessedPanel(g, innerBounds);
    
    // Bottom rack screws (4 screws across the bottom)
    float bottomY = bounds.getHeight() - 18.0f;
    float screwSpacing = bounds.getWidth() / 5.0f;
    for (int i = 1; i <= 4; ++i)
    {
        MetallicLookAndFeel::drawCornerScrew(g, juce::Point<float>(screwSpacing * i, bottomY), 6.0f);
    }
    
    // TOP HEADER BAR - Draw embossed header background
    auto headerBounds = innerBounds.reduced(6).removeFromTop(60);
    MetallicLookAndFeel::drawDeeplyRecessedSection(g, headerBounds);
    
    // Draw RHYTHMIC SPACE title with icon
    auto titleArea = headerBounds.reduced(16);
    g.setColour(juce::Colours::white);
    
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #endif
    
    juce::Font titleFont(18.0f, juce::Font::plain);
    g.setFont(titleFont);
    
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
    
    // Title text with glow effect
    g.setColour(juce::Colour(0xffffffff));
    auto titleTextArea = titleArea.removeFromLeft(250);
    g.drawText("RHYTHMIC SPACE", titleTextArea.removeFromTop(22), 
               juce::Justification::left, false);
    
    g.setColour(juce::Colour(0xff9ca3af)); // text-zinc-400
    
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #endif
    
    juce::Font subtitleFont(10.0f, juce::Font::plain);
    g.setFont(subtitleFont);
    
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif
    
    g.drawText("Step Sequencer Multi-Effect Processor", titleTextArea, 
               juce::Justification::left, false);
    
    // Draw LATHAMAUDIO embossed badge on the right (much wider to fit full text)
    auto badgeBounds = titleArea.removeFromRight(260).reduced(2, 8); // Increased from 220 to 260
    
    // Debug log to check bounds
    DBG("Badge bounds: " << badgeBounds.toString());
    
    MetallicLookAndFeel::drawEmbossedBadge(g, badgeBounds, "LATHAM", "AUDIO");
    
    // TRANSPORT SECTION - Recessed panel
    auto transportBounds = innerBounds.reduced(6);
    transportBounds.removeFromTop(66); // Skip header
    auto transportPanel = transportBounds.removeFromTop(60);
    MetallicLookAndFeel::drawDeeplyRecessedSection(g, transportPanel);
    
    // PATTERN SEQUENCER SECTION - Large recessed panel
    transportBounds.removeFromTop(6);
    auto sequencerPanel = transportBounds.removeFromLeft(706).removeFromTop(530);
    MetallicLookAndFeel::drawDeeplyRecessedSection(g, sequencerPanel);
    
    // RIGHT COLUMN PANELS
    transportBounds.removeFromLeft(6);
    auto rightColumn = transportBounds;
    
    // Preset browser panel (reduced - category dropdown removed)
    auto presetPanel = rightColumn.removeFromTop(155);
    MetallicLookAndFeel::drawDeeplyRecessedSection(g, presetPanel);
    rightColumn.removeFromTop(6);
    
    // Effect panel - Taller to show all knob labels properly
    auto effectPanel = rightColumn.removeFromTop(200);
    MetallicLookAndFeel::drawDeeplyRecessedSection(g, effectPanel);
    rightColumn.removeFromTop(6);
    
    // MIDI learn panel background
    auto midiPanel = rightColumn.removeFromTop(160);  // Increased height for full component
    MetallicLookAndFeel::drawDeeplyRecessedSection(g, midiPanel);
    
    // No more separate master section - meters and knob are now in the volume lane!
    
    // Position resizable corner in bottom-right
    resizableCorner->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
}

void RhythmicSpaceAudioProcessorEditor::resized()
{
    // Account for outer chassis (12px padding) + inner panel (6px padding)
    auto bounds = getLocalBounds().reduced(18);
    
    // Header area (drawn in paint, contains title and badge)
    bounds.removeFromTop(60); // Reserve space for header
    bounds.removeFromTop(6); // Spacing after header
    
    // Transport section (recessed panel) 
    auto transportBounds = bounds.removeFromTop(60);
    auto transportInner = transportBounds.reduced(12); // Padding inside recessed panel
    transport->setBounds(transportInner);
    
    bounds.removeFromTop(6);
    
    // Main content area
    auto contentArea = bounds;
    
    // Left column: Step Sequencers in recessed panel (5 lanes)
    auto sequencerArea = contentArea.removeFromLeft(706);
    auto sequencerInner = sequencerArea.removeFromTop(530).reduced(12); // Padding inside panel
    
    int seqHeight = 96;
    int seqSpacing = 6;
    
    filterStepSequencer->setBounds(sequencerInner.removeFromTop(seqHeight));
    sequencerInner.removeFromTop(seqSpacing);
    
    panStepSequencer->setBounds(sequencerInner.removeFromTop(seqHeight));
    sequencerInner.removeFromTop(seqSpacing);
    
    delayStepSequencer->setBounds(sequencerInner.removeFromTop(seqHeight));
    sequencerInner.removeFromTop(seqSpacing);
    
    reverbStepSequencer->setBounds(sequencerInner.removeFromTop(seqHeight));
    sequencerInner.removeFromTop(seqSpacing);
    
    // Volume lane WITH meters and master knob integrated on the right
    auto volumeLane = sequencerInner.removeFromTop(seqHeight);
    
    // Put meters and master on the right side of the volume lane
    auto masterControls = volumeLane.removeFromRight(110); // Space for meters + knob
    volumeStepSequencer->setBounds(volumeLane.removeFromLeft(volumeLane.getWidth() - 6)); // Leave gap
    
    // Layout meters and master knob vertically centered in the volume lane
    masterControls.removeFromTop(20); // Top margin to center vertically
    auto masterRow = masterControls.removeFromTop(56); // Height for knob
    
    inputMeter->setBounds(masterRow.removeFromLeft(18));
    masterRow.removeFromLeft(4);
    outputMeter->setBounds(masterRow.removeFromLeft(18));
    masterRow.removeFromLeft(6);
    masterVolumeKnob->setBounds(masterRow.removeFromLeft(56));
    
    // Right column: Controls and metering
    contentArea.removeFromLeft(6);
    auto rightColumn = contentArea;
    
    // Preset browser at top right (in recessed panel) - compact
    auto presetPanel = rightColumn.removeFromTop(155);
    presetBrowser->setBounds(presetPanel.reduced(12));
    rightColumn.removeFromTop(6);
    
    // Effect panel (in recessed panel) - Taller to show all labels
    auto effectPanelBounds = rightColumn.removeFromTop(200);
    effectPanel->setBounds(effectPanelBounds.reduced(10));
    rightColumn.removeFromTop(6);
    
    // MIDI Learn panel (in recessed panel) - ENABLED! 🎛️
    auto midiPanel = rightColumn.removeFromTop(160);
    midiLearn->setBounds(midiPanel.reduced(12));
    
    // No more separate master section - meters and knob are now in the volume lane!
    
    // Position resizable corner in bottom-right
    resizableCorner->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
}

//==============================================================================
void RhythmicSpaceAudioProcessorEditor::timerCallback()
{
    // Update current step indicator in all sequencer lanes
    int currentStep = audioProcessor.getCurrentStep();
    
    filterStepSequencer->setCurrentStep(currentStep);
    panStepSequencer->setCurrentStep(currentStep);
    delayStepSequencer->setCurrentStep(currentStep);
    reverbStepSequencer->setCurrentStep(currentStep);
    volumeStepSequencer->setCurrentStep(currentStep);
    
    // Update level meters
    inputMeter->setLevel(audioProcessor.getInputLevel());
    outputMeter->setLevel(audioProcessor.getOutputLevel());
    
    // Check for MIDI activity and flash indicator
    if (audioProcessor.hasMIDIActivity())
    {
        if (midiLearn)
            midiLearn->flashMIDIActivity();
        audioProcessor.clearMIDIActivity();
    }
    
    // Sync step values from processor to UI
    auto& seq = audioProcessor.getStepSequencer();
    for (int i = 0; i < 16; ++i)
    {
        filterStepSequencer->setStepValue(i, seq.getStepValue(StepSequencer::FilterParam, i));
        panStepSequencer->setStepValue(i, seq.getStepValue(StepSequencer::PanParam, i));
        delayStepSequencer->setStepValue(i, seq.getStepValue(StepSequencer::DelayParam, i));
        reverbStepSequencer->setStepValue(i, seq.getStepValue(StepSequencer::ReverbParam, i));
        volumeStepSequencer->setStepValue(i, seq.getStepValue(StepSequencer::VolumeParam, i));
    }
}

//==============================================================================
void RhythmicSpaceAudioProcessorEditor::mouseDown(const juce::MouseEvent& e)
{
    // Window dragging disabled - plugin should remain fixed in host
    // (Window dragging is only appropriate for standalone applications, not plugins)
}

void RhythmicSpaceAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e)
{
    // Window dragging disabled - plugin should remain fixed in host
    // (Window dragging is only appropriate for standalone applications, not plugins)
}