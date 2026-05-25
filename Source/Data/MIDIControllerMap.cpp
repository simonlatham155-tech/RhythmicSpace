#include "MIDIControllerMap.h"

//==============================================================================
MIDIControllerMap::MIDIControllerMap()
{
}

//==============================================================================
void MIDIControllerMap::startLearning(const juce::String& parameterID)
{
    learningParameterID = parameterID;
    DBG("MIDI Learn started for: " + parameterID);
}

void MIDIControllerMap::stopLearning()
{
    learningParameterID = "";
    DBG("MIDI Learn stopped");
}

//==============================================================================
bool MIDIControllerMap::processMIDIMessage(const juce::MidiMessage& message,
                                          std::function<void(const juce::String&, float)> parameterCallback)
{
    // Only process CC messages
    if (!message.isController())
        return false;
    
    int ccNumber = message.getControllerNumber();
    int ccValue = message.getControllerValue();
    int channel = message.getChannel();
    
    DBG("MIDI CC received: CC" + juce::String(ccNumber) + 
        " value=" + juce::String(ccValue) + 
        " channel=" + juce::String(channel));
    
    // MIDI Learn mode - assign this CC to the learning parameter
    if (isLearning())
    {
        addMapping(ccNumber, channel, learningParameterID);
        stopLearning();
        DBG("Learned mapping: CC" + juce::String(ccNumber) + " → " + learningParameterID);
        return true;
    }
    
    // Normal mode - find mapping and update parameter
    if (auto* mapping = findMappingByCC(ccNumber, channel))
    {
        // Convert MIDI value (0-127) to normalized value (0-1)
        float normalizedValue = ccValue / 127.0f;
        
        // Scale to parameter range
        float scaledValue = mapping->minValue + normalizedValue * (mapping->maxValue - mapping->minValue);
        
        // Call parameter callback
        if (parameterCallback)
        {
            parameterCallback(mapping->parameterID, scaledValue);
            return true;
        }
    }
    
    return false;
}

//==============================================================================
void MIDIControllerMap::addMapping(int ccNumber, int channel, const juce::String& parameterID,
                                  float minValue, float maxValue)
{
    // Remove existing mapping for this parameter
    removeMapping(parameterID);
    
    // Add new mapping
    MIDIMapping mapping;
    mapping.ccNumber = ccNumber;
    mapping.channel = channel;
    mapping.parameterID = parameterID;
    mapping.minValue = minValue;
    mapping.maxValue = maxValue;
    
    mappings.push_back(mapping);
    
    DBG("Added mapping: CC" + juce::String(ccNumber) + 
        " (Ch " + juce::String(channel) + ") → " + parameterID);
}

void MIDIControllerMap::removeMapping(const juce::String& parameterID)
{
    mappings.erase(
        std::remove_if(mappings.begin(), mappings.end(),
            [&parameterID](const MIDIMapping& m) { return m.parameterID == parameterID; }),
        mappings.end()
    );
}

void MIDIControllerMap::clearAllMappings()
{
    mappings.clear();
    DBG("Cleared all MIDI mappings");
}

//==============================================================================
MIDIControllerMap::MIDIMapping* MIDIControllerMap::getMapping(const juce::String& parameterID)
{
    for (auto& mapping : mappings)
    {
        if (mapping.parameterID == parameterID)
            return &mapping;
    }
    return nullptr;
}

const MIDIControllerMap::MIDIMapping* MIDIControllerMap::getMapping(const juce::String& parameterID) const
{
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
        // Match CC number and channel (or omni channel -1)
        if (mapping.ccNumber == ccNumber && 
            (mapping.channel == -1 || mapping.channel == channel))
        {
            return &mapping;
        }
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
    DBG("Loading Arturia MiniLab 3 template");
    
    // Arturia MiniLab 3 default CC mappings (User Mode)
    // These can be customized in Arturia MIDI Control Center
    
    // 8 Rotary Encoders (endless, relative mode typically CC 70-77)
    // For this template, we'll use absolute mode CC 7, 74, 71, 76, 77, 93, 73, 75
    
    int channel = -1; // Omni (accept any channel)
    
    // Knob 1 (CC 74) → Filter Cutoff
    addMapping(74, channel, "filterCutoff");
    
    // Knob 2 (CC 71) → Filter Resonance
    addMapping(71, channel, "filterResonance");
    
    // Knob 3 (CC 76) → Filter Mix
    addMapping(76, channel, "filterMix");
    
    // Knob 4 (CC 77) → Delay Time
    addMapping(77, channel, "delayTime");
    
    // Knob 5 (CC 93) → Delay Feedback
    addMapping(93, channel, "delayFeedback");
    
    // Knob 6 (CC 73) → Delay Mix
    addMapping(73, channel, "delayMix");
    
    // Knob 7 (CC 75) → Reverb Size
    addMapping(75, channel, "reverbSize");
    
    // Knob 8 (CC 18) → Reverb Mix
    addMapping(18, channel, "reverbMix");
    
    // Mod Wheel (CC 1) → Pan Width
    addMapping(1, channel, "panWidth");
    
    // Volume Slider (CC 7) → Master Volume
    addMapping(7, channel, "masterVolume");
    
    // Pads can be configured to send CC messages in Arturia MIDI Control Center
    // For now, we'll leave them for step triggering via MIDI notes
    
    DBG("Arturia MiniLab 3 template loaded with " + juce::String(mappings.size()) + " mappings");
}

//==============================================================================
juce::ValueTree MIDIControllerMap::toValueTree() const
{
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
    if (!tree.hasType("MIDIControllerMap"))
        return;
    
    clearAllMappings();
    
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
    
    DBG("Loaded " + juce::String(mappings.size()) + " MIDI mappings from preset");
}