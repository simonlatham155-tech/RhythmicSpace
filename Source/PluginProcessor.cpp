#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RhythmicSpaceAudioProcessor::RhythmicSpaceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                     #if JucePlugin_WantsMidiInput
                       .withInput  ("MIDI In", juce::AudioChannelSet::disabled())
                     #endif
                       ),
#endif
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Don't load preset in constructor - causes JUCE assertion
    // Parameters are already initialized with defaults from createParameterLayout()
}

RhythmicSpaceAudioProcessor::~RhythmicSpaceAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout RhythmicSpaceAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Transport
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("playing", 1), "Playing", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bpm", 1), "BPM", 60.0f, 240.0f, 120.0f));
    
    // Filter parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filterCutoff", 1), "Filter Cutoff", 20.0f, 20000.0f, 1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filterResonance", 1), "Filter Resonance", 0.1f, 10.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("filterType", 1), "Filter Type", 
        juce::StringArray{"Low Pass", "High Pass", "Band Pass"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filterMix", 1), "Filter Mix", 0.0f, 100.0f, 50.0f));
    
    // Delay parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayTime", 1), "Delay Time", 0.0f, 2000.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayFeedback", 1), "Delay Feedback", 0.0f, 95.0f, 40.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayMix", 1), "Delay Mix", 0.0f, 100.0f, 30.0f));
    
    // Reverb parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbSize", 1), "Reverb Size", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbDamping", 1), "Reverb Damping", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbMix", 1), "Reverb Mix", 0.0f, 100.0f, 25.0f));
    
    // Pan parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("panWidth", 1), "Pan Width", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("panRate", 1), "Pan Rate", 0.0f, 100.0f, 50.0f));
    
    // Volume parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("volumeAmount", 1), "Volume Amount", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("masterVolume", 1), "Master Volume", 0.0f, 100.0f, 80.0f));
    
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String RhythmicSpaceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RhythmicSpaceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RhythmicSpaceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RhythmicSpaceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RhythmicSpaceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RhythmicSpaceAudioProcessor::getNumPrograms()
{
    return presetManager.getNumPresets();
}

int RhythmicSpaceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RhythmicSpaceAudioProcessor::setCurrentProgram (int index)
{
    loadPreset(index);
}

const juce::String RhythmicSpaceAudioProcessor::getProgramName (int index)
{
    if (auto* preset = presetManager.getPreset(index))
        return preset->name;
    return "Empty";
}

void RhythmicSpaceAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RhythmicSpaceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Debug MIDI acceptance
    DBG("Plugin acceptsMidi: " + juce::String(acceptsMidi() ? "YES" : "NO"));
    DBG("Plugin isMidiEffect: " + juce::String(isMidiEffect() ? "YES" : "NO"));
    
    // Prepare all processors
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(samplesPerBlock);
    spec.numChannels = 2;
    
    stepSequencer.prepare(sampleRate, bpm);
    filterProcessor.prepare(spec);
    delayProcessor.prepare(spec);
    reverbProcessor.prepare(spec);
    panProcessor.prepare(spec);
    volumeProcessor.prepare(spec);
}

void RhythmicSpaceAudioProcessor::releaseResources()
{
    // Release resources when playback stops
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RhythmicSpaceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Stereo only
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RhythmicSpaceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Process MIDI messages for controller mapping 🎛️
    if (!midiMessages.isEmpty())
    {
        DBG("Processing " + juce::String(midiMessages.getNumEvents()) + " MIDI messages");
        
        // Set MIDI activity flag for UI indicator
        midiActivityFlag.store(true);
    }
    
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        midiControllerMap.processMIDIMessage(message, [this](const juce::String& paramID, float value)
        {
            // Update parameter from MIDI controller
            if (auto* param = parameters.getParameter(paramID))
            {
                param->setValueNotifyingHost(value);
            }
        });
    }

    // Measure input level
    float inputPeak = buffer.getMagnitude(0, buffer.getNumSamples());
    inputLevel.store(inputPeak);

    // Sync to host tempo if enabled
    if (hostSyncEnabled.load())
    {
        if (auto* playHead = getPlayHead())
        {
            if (auto position = playHead->getPosition())
            {
                if (position->getBpm().hasValue())
                {
                    double hostBpm = *position->getBpm();
                    if (hostBpm != bpm.load())
                    {
                        bpm.store(hostBpm);
                        stepSequencer.setBPM(hostBpm);
                    }
                }
            }
        }
    }

    // Update step sequencer if playing
    if (playing.load())
    {
        stepSequencer.process(buffer.getNumSamples());
    }

    // Get current modulation values from step sequencer
    auto modValues = stepSequencer.getCurrentModulationValues();

    // Process audio through effect chain
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Apply effects with step sequencer modulation
    filterProcessor.process(context, modValues.filter, 
        parameters.getRawParameterValue("filterCutoff")->load(),
        parameters.getRawParameterValue("filterResonance")->load(),
        parameters.getRawParameterValue("filterType")->load(),
        parameters.getRawParameterValue("filterMix")->load() / 100.0f);

    delayProcessor.process(context, modValues.delay,
        parameters.getRawParameterValue("delayTime")->load(),
        parameters.getRawParameterValue("delayFeedback")->load() / 100.0f,
        parameters.getRawParameterValue("delayMix")->load() / 100.0f);

    reverbProcessor.process(context, modValues.reverb,
        parameters.getRawParameterValue("reverbSize")->load() / 100.0f,
        parameters.getRawParameterValue("reverbDamping")->load() / 100.0f,
        parameters.getRawParameterValue("reverbMix")->load() / 100.0f);

    panProcessor.process(context, modValues.pan,
        parameters.getRawParameterValue("panWidth")->load() / 100.0f,
        parameters.getRawParameterValue("panRate")->load() / 100.0f);

    volumeProcessor.process(context, modValues.volume,
        parameters.getRawParameterValue("volumeAmount")->load() / 100.0f);

    // Apply master volume
    float masterVol = parameters.getRawParameterValue("masterVolume")->load() / 100.0f;
    buffer.applyGain(masterVol);

    // Measure output level
    float outputPeak = buffer.getMagnitude(0, buffer.getNumSamples());
    outputLevel.store(outputPeak);
}

//==============================================================================
bool RhythmicSpaceAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* RhythmicSpaceAudioProcessor::createEditor()
{
    return new RhythmicSpaceAudioProcessorEditor (*this);
}

//==============================================================================
void RhythmicSpaceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save state
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    
    // Add step sequencer state
    auto* stepSeqXml = xml->createNewChildElement("StepSequencer");
    stepSequencer.saveState(*stepSeqXml);
    
    copyXmlToBinary(*xml, destData);
}

void RhythmicSpaceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Load state
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
            
            // Load step sequencer state
            if (auto* stepSeqXml = xmlState->getChildByName("StepSequencer"))
                stepSequencer.loadState(*stepSeqXml);
        }
    }
}

//==============================================================================
void RhythmicSpaceAudioProcessor::setPlaying(bool shouldPlay)
{
    playing.store(shouldPlay);
    if (shouldPlay)
        stepSequencer.reset();
}

void RhythmicSpaceAudioProcessor::setBPM(double newBPM)
{
    bpm.store(newBPM);
    stepSequencer.setBPM(newBPM);
    parameters.getParameter("bpm")->setValueNotifyingHost((newBPM - 60.0f) / 180.0f);
}

void RhythmicSpaceAudioProcessor::setHostSyncEnabled(bool enabled)
{
    hostSyncEnabled.store(enabled);
}

void RhythmicSpaceAudioProcessor::loadPreset(int presetIndex)
{
    if (auto* preset = presetManager.getPreset(presetIndex))
    {
        // Load preset into step sequencer
        stepSequencer.loadPreset(*preset);
        
        // Update parameters
        setBPM(preset->bpm);
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RhythmicSpaceAudioProcessor();
}