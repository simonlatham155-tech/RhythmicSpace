#include "MIDIControllerMap.h"

//==============================================================================
MIDIControllerMap::MIDIControllerMap()
{
}

void MIDIControllerMap::startLearning(const juce::String& parameterID)
{
    const juce::ScopedLock lock(mappingLock);
    learningParameterID = parameterID;
}

void MIDIControllerMap::stopLearning()
{
    const juce::ScopedLock lock(mappingLock);
    learningParameterID = {};
}

bool MIDIControllerMap::isLearning() const
{
    const juce::ScopedLock lock(mappingLock);
    return learningParameterID.isNotEmpty();
}

juce::String MIDIControllerMap::getLearningParameterID() const
{
    const juce::ScopedLock lock(mappingLock);
    return learningParameterID;
}

//==============================================================================
bool MIDIControllerMap::processMIDIMessage(const juce::MidiMessage& message,
                                          std::function<void(const juce::String&, float)> parameterCallback)
{
    if (! message.isController())
        return false;
    
    const int ccNumber = message.getControllerNumber();
    const int ccValue = message.getControllerValue();
    const int channel = message.getChannel();

    juce::String learningParameter;
    MIDIMapping matchedMapping;
    bool hasMatchedMapping = false;

    {
        const juce::ScopedLock lock(mappingLock);

        if (learningParameterID.isNotEmpty())
        {
            learningParameter = learningParameterID;
            learningParameterID = {};

            MIDIMapping mapping;
            mapping.ccNumber = ccNumber;
            mapping.channel = channel;
            mapping.parameterID = learningParameter;
            mapping.minValue = 0.0f;
            mapping.maxValue = 1.0f;

            mappings.erase(
                std::remove_if(mappings.begin(), mappings.end(),
                    [&learningParameter](const MIDIMapping& m) { return m.parameterID == learningParameter; }),
                mappings.end());

            mappings.push_back(mapping);
        }
        else if (auto* mapping = findMappingByCC(ccNumber, channel))
        {
            matchedMapping = *mapping;
            hasMatchedMapping = true;
        }
    }

    if (learningParameter.isNotEmpty())
        return true;

    if (hasMatchedMapping)
    {
        const float normalizedValue = ccValue / 127.0f;
        const float scaledValue = matchedMapping.minValue
                                + normalizedValue * (matchedMapping.maxValue - matchedMapping.minValue);

        if (parameterCallback)
        {
            parameterCallback(matchedMapping.parameterID, scaledValue);
            return true;
        }
    }
    
    return false;
}

//==============================================================================
void MIDIControllerMap::addMapping(int ccNumber, int channel, const juce::String& parameterID,
                                  float minValue, float maxValue)
{
    const juce::ScopedLock lock(mappingLock);

    mappings.erase(
        std::remove_if(mappings.begin(), mappings.end(),
            [&parameterID](const MIDIMapping& m) { return m.parameterID == parameterID; }),
        mappings.end());

    MIDIMapping mapping;
    mapping.ccNumber = ccNumber;
    mapping.channel = channel;
    mapping.parameterID = parameterID;
    mapping.minValue = minValue;
    mapping.maxValue = maxValue;

    mappings.push_back(mapping);
}

void MIDIControllerMap::removeMapping(const juce::String& parameterID)
{
    const juce::ScopedLock lock(mappingLock);

    mappings.erase(
        std::remove_if(mappings.begin(), mappings.end(),
            [&parameterID](const MIDIMapping& m) { return m.parameterID == parameterID; }),
        mappings.end());
}

void MIDIControllerMap::clearAllMappings()
{
    const juce::ScopedLock lock(mappingLock);
    mappings.clear();
}

//==============================================================================
MIDIControllerMap::MIDIMapping* MIDIControllerMap::getMapping(const juce::String& parameterID)
{
    const juce::ScopedLock lock(mappingLock);

    for (auto& mapping : mappings)
    {
        if (mapping.parameterID == parameterID)
            return &mapping;
    }

    return nullptr;
}

const MIDIControllerMap::MIDIMapping* MIDIControllerMap::getMapping(const juce::String& parameterID) const
{
    const juce::ScopedLock lock(mappingLock);

    for (const auto& mapping : mappings)
    {
        if (mapping.parameterID == parameterID)
            return &mapping;
    }

    return nullptr;
}

MIDIControllerMap::MIDIMapping* MIDIControllerMap::findMappingByCC(int ccNumber, int channel)
{
    for (auto& mapping : mappings)
    {
        if (mapping.ccNumber == ccNumber
            && (mapping.channel == -1 || mapping.channel == channel))
            return &mapping;
    }

    return nullptr;
}

//==============================================================================
void MIDIControllerMap::loadTemplate(ControllerTemplate templateType)
{
    clearAllMappings();
    
    switch (templateType)
    {
        case ControllerTemplate::ArturiaMiniLab3:
            loadArturiaMiniLab3Template();
            break;
            
        case ControllerTemplate::None:
        case ControllerTemplate::CustomMapping:
        default:
            break;
    }
}

void MIDIControllerMap::loadArturiaMiniLab3Template()
{
    const int channel = -1;

    addMapping(74, channel, "filterCutoff");
    addMapping(71, channel, "filterResonance");
    addMapping(76, channel, "filterMix");
    addMapping(77, channel, "delayTime");
    addMapping(93, channel, "delayFeedback");
    addMapping(73, channel, "delayMix");
    addMapping(75, channel, "reverbSize");
    addMapping(18, channel, "reverbMix");
    addMapping(1, channel, "panWidth");
    addMapping(7, channel, "masterVolume");
}

//==============================================================================
juce::ValueTree MIDIControllerMap::toValueTree() const
{
    const juce::ScopedLock lock(mappingLock);
    juce::ValueTree tree("MIDIControllerMap");
    
    for (const auto& mapping : mappings)
    {
        juce::ValueTree mappingTree("Mapping");
        mappingTree.setProperty("ccNumber", mapping.ccNumber, nullptr);
        mappingTree.setProperty("channel", mapping.channel, nullptr);
        mappingTree.setProperty("parameterID", mapping.parameterID, nullptr);
        mappingTree.setProperty("minValue", mapping.minValue, nullptr);
        mappingTree.setProperty("maxValue", mapping.maxValue, nullptr);
        
        tree.appendChild(mappingTree, nullptr);
    }
    
    return tree;
}

void MIDIControllerMap::fromValueTree(const juce::ValueTree& tree)
{
    if (! tree.hasType("MIDIControllerMap"))
        return;
    
    const juce::ScopedLock lock(mappingLock);
    mappings.clear();
    
    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        auto mappingTree = tree.getChild(i);
        
        if (mappingTree.hasType("Mapping"))
        {
            MIDIMapping mapping;
            mapping.ccNumber = mappingTree.getProperty("ccNumber", -1);
            mapping.channel = mappingTree.getProperty("channel", -1);
            mapping.parameterID = mappingTree.getProperty("parameterID", "");
            mapping.minValue = mappingTree.getProperty("minValue", 0.0f);
            mapping.maxValue = mappingTree.getProperty("maxValue", 1.0f);
            
            if (mapping.isValid())
                mappings.push_back(mapping);
        }
    }
}
