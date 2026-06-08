#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    void setParameterValue(juce::AudioProcessorValueTreeState& apvts,
                           const juce::String& parameterID,
                           float value)
    {
        if (auto* param = apvts.getParameter(parameterID))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    }
}

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
}

RhythmicSpaceAudioProcessor::~RhythmicSpaceAudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout RhythmicSpaceAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("playing", 1), "Playing", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("bpm", 1), "BPM", 60.0f, 240.0f, 120.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filterCutoff", 1), "Filter Cutoff", 20.0f, 20000.0f, 1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filterResonance", 1), "Filter Resonance", 0.1f, 10.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("filterType", 1), "Filter Type", 
        juce::StringArray{"Low Pass", "High Pass", "Band Pass"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("filterMix", 1), "Filter Mix", 0.0f, 100.0f, 50.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayTime", 1), "Delay Time", 0.0f, 2000.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayFeedback", 1), "Delay Feedback", 0.0f, 95.0f, 40.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("delayMix", 1), "Delay Mix", 0.0f, 100.0f, 30.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbSize", 1), "Reverb Size", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbDamping", 1), "Reverb Damping", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("reverbMix", 1), "Reverb Mix", 0.0f, 100.0f, 25.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("panWidth", 1), "Pan Width", 0.0f, 100.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("panRate", 1), "Pan Rate", 0.0f, 100.0f, 50.0f));
    
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
    const float delaySeconds = parameters.getRawParameterValue("delayTime")->load() / 1000.0f;
    const float feedback = parameters.getRawParameterValue("delayFeedback")->load() / 100.0f;
    const float delayTail = delaySeconds * (1.0f + feedback * 4.0f);
    constexpr double reverbTail = 3.0;
    return static_cast<double>(delayTail) + reverbTail;
}

int RhythmicSpaceAudioProcessor::getNumPrograms()
{
    return presetManager.getNumPresets();
}

int RhythmicSpaceAudioProcessor::getCurrentProgram()
{
    return currentProgramIndex;
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
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void RhythmicSpaceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(samplesPerBlock);
    spec.numChannels = 2;
    
    stepSequencer.prepare(sampleRate, bpm.load());
    modulationSmoother.prepare(sampleRate);
    filterProcessor.prepare(spec);
    delayProcessor.prepare(spec);
    reverbProcessor.prepare(spec);
    panProcessor.prepare(spec);
    volumeProcessor.prepare(spec);
}

void RhythmicSpaceAudioProcessor::releaseResources()
{
    filterProcessor.reset();
    delayProcessor.reset();
    reverbProcessor.reset();
    panProcessor.reset();
    volumeProcessor.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RhythmicSpaceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
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

void RhythmicSpaceAudioProcessor::updateHostTransportState()
{
    if (! hostSyncEnabled.load())
        return;

    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            if (position->getBpm().hasValue())
            {
                const double hostBpm = *position->getBpm();

                if (hostBpm != bpm.load())
                {
                    bpm.store(hostBpm);
                    stepSequencer.setBPM(hostBpm);
                }
            }

            playing.store(position->getIsPlaying());

            if (position->getPpqPosition().hasValue())
            {
                const double hostPpq = *position->getPpqPosition();
                const bool hostPlaying = playing.load();
                const bool hostJumped = lastHostPpq < 0.0
                                     || std::abs(hostPpq - lastHostPpq) > 0.25;

                if (hostPlaying || hostJumped)
                    stepSequencer.syncToHostPpq(hostPpq);

                lastHostPpq = hostPpq;
            }
        }
    }
}

int RhythmicSpaceAudioProcessor::getFilterTypeIndex() const
{
    if (auto* typeParam = dynamic_cast<juce::AudioParameterChoice*>(parameters.getParameter("filterType")))
        return typeParam->getIndex();

    return 0;
}

void RhythmicSpaceAudioProcessor::processEffectChain(juce::dsp::ProcessContextReplacing<float>& context,
                                                     const ModulationValues& modValues)
{
    filterProcessor.process(context, modValues.filter,
        parameters.getRawParameterValue("filterCutoff")->load(),
        parameters.getRawParameterValue("filterResonance")->load(),
        getFilterTypeIndex(),
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
}

void RhythmicSpaceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (! midiMessages.isEmpty())
        midiActivityFlag.store(true);
    
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        midiControllerMap.processMIDIMessage(message, [this](const juce::String& paramID, float value)
        {
            if (auto* param = parameters.getParameter(paramID))
                param->setValueNotifyingHost(value);
        });
    }

    float inputPeak = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        inputPeak = juce::jmax(inputPeak, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));

    inputLevel.store(inputPeak);

    updateHostTransportState();

    const float masterVol = parameters.getRawParameterValue("masterVolume")->load() / 100.0f;
    int startSample = 0;
    const int numSamples = buffer.getNumSamples();

    while (startSample < numSamples)
    {
        int samplesToProcess = stepSequencer.getSamplesUntilNextStep();
        samplesToProcess = juce::jmin(samplesToProcess, numSamples - startSample);

        if (! hostSyncEnabled.load() && playing.load())
            stepSequencer.advance(samplesToProcess);

        const auto modValues = stepSequencer.getCurrentModulationValues();
        modulationSmoother.setTargets(modValues);
        const auto smoothedMod = modulationSmoother.advance(samplesToProcess);

        juce::dsp::AudioBlock<float> fullBlock(buffer);
        auto subBlock = fullBlock.getSubBlock((size_t) startSample, (size_t) samplesToProcess);
        juce::dsp::ProcessContextReplacing<float> context(subBlock);

        processEffectChain(context, smoothedMod);
        subBlock.multiplyBy(masterVol);

        startSample += samplesToProcess;
    }

    float outputPeak = 0.0f;
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
        outputPeak = juce::jmax(outputPeak, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));

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
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    
    auto* stepSeqXml = xml->createNewChildElement("StepSequencer");
    stepSequencer.saveState(*stepSeqXml);

    xml->setAttribute("hostSyncEnabled", hostSyncEnabled.load());
    xml->setAttribute("currentProgram", currentProgramIndex);

    if (auto midiXml = midiControllerMap.toValueTree().createXml())
        xml->addChildElement(midiXml.release());
    
    copyXmlToBinary(*xml, destData);
}

void RhythmicSpaceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
            
            if (auto* stepSeqXml = xmlState->getChildByName("StepSequencer"))
                stepSequencer.loadState(*stepSeqXml);

            if (auto* midiXml = xmlState->getChildByName("MIDIControllerMap"))
                midiControllerMap.fromValueTree(juce::ValueTree::fromXml(*midiXml));

            hostSyncEnabled.store(xmlState->getBoolAttribute("hostSyncEnabled", false));
            currentProgramIndex = xmlState->getIntAttribute("currentProgram", currentProgramIndex);

            syncRuntimeStateFromParameters();
        }
    }
}

void RhythmicSpaceAudioProcessor::syncRuntimeStateFromParameters()
{
    if (auto* playingParam = parameters.getRawParameterValue("playing"))
        playing.store(playingParam->load() >= 0.5f);

    if (auto* bpmParam = parameters.getRawParameterValue("bpm"))
    {
        const double newBpm = bpmParam->load();
        bpm.store(newBpm);
        stepSequencer.setBPM(newBpm);
    }
}

void RhythmicSpaceAudioProcessor::applyPresetParameters(const Preset& preset)
{
    setParameterValue(parameters, "filterCutoff", preset.filterCutoff);
    setParameterValue(parameters, "filterResonance", preset.filterResonance);
    setParameterValue(parameters, "filterMix", preset.filterMix);
    setParameterValue(parameters, "delayTime", preset.delayTime);
    setParameterValue(parameters, "delayFeedback", preset.delayFeedback);
    setParameterValue(parameters, "delayMix", preset.delayMix);
    setParameterValue(parameters, "reverbSize", preset.reverbSize);
    setParameterValue(parameters, "reverbDamping", preset.reverbDamping);
    setParameterValue(parameters, "reverbMix", preset.reverbMix);
    setParameterValue(parameters, "panWidth", preset.panWidth);
    setParameterValue(parameters, "panRate", preset.panRate);
    setParameterValue(parameters, "volumeAmount", preset.volumeAmount);
    setParameterValue(parameters, "masterVolume", preset.masterVolume);

    if (auto* typeParam = dynamic_cast<juce::AudioParameterChoice*>(parameters.getParameter("filterType")))
        typeParam->setValueNotifyingHost(typeParam->convertTo0to1((float) juce::jlimit(0, 2, preset.filterType)));
}

//==============================================================================
void RhythmicSpaceAudioProcessor::setPlaying(bool shouldPlay)
{
    playing.store(shouldPlay);

    if (auto* param = parameters.getParameter("playing"))
        param->setValueNotifyingHost(shouldPlay ? 1.0f : 0.0f);

    if (shouldPlay)
        stepSequencer.reset();
}

void RhythmicSpaceAudioProcessor::setBPM(double newBPM)
{
    bpm.store(newBPM);
    stepSequencer.setBPM(newBPM);
    setParameterValue(parameters, "bpm", static_cast<float>(newBPM));
}

void RhythmicSpaceAudioProcessor::setHostSyncEnabled(bool enabled)
{
    hostSyncEnabled.store(enabled);

    if (enabled)
        lastHostPpq = -1.0;
}

void RhythmicSpaceAudioProcessor::loadPreset(int presetIndex)
{
    if (auto* preset = presetManager.getPreset(presetIndex))
    {
        stepSequencer.loadPreset(*preset);
        applyPresetParameters(*preset);
        setBPM(preset->bpm);
        currentProgramIndex = presetIndex;
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RhythmicSpaceAudioProcessor();
}
