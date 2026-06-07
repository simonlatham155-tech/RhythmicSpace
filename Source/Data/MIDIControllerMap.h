#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * MIDI Controller Mapping System
 */
class MIDIControllerMap
{
public:
    struct MIDIMapping
    {
        int ccNumber = -1;
        int channel = -1;
        juce::String parameterID;
        float minValue = 0.0f;
        float maxValue = 1.0f;
        bool isLearning = false;
        
        bool isValid() const { return ccNumber >= 0 && ccNumber <= 127; }
    };
    
    enum class ControllerTemplate
    {
        None,
        ArturiaMiniLab3,
        LaunchpadMini,
        CustomMapping
    };
    
    MIDIControllerMap();
    ~MIDIControllerMap() = default;
    
    void startLearning(const juce::String& parameterID);
    void stopLearning();
    bool isLearning() const;
    juce::String getLearningParameterID() const;
    
    bool processMIDIMessage(const juce::MidiMessage& message, 
                           std::function<void(const juce::String&, float)> parameterCallback);
    
    void addMapping(int ccNumber, int channel, const juce::String& parameterID,
                   float minValue = 0.0f, float maxValue = 1.0f);
    void removeMapping(const juce::String& parameterID);
    void clearAllMappings();
    MIDIMapping* getMapping(const juce::String& parameterID);
    const MIDIMapping* getMapping(const juce::String& parameterID) const;
    const std::vector<MIDIMapping>& getAllMappings() const { return mappings; }
    
    void loadTemplate(ControllerTemplate templateType);
    void loadArturiaMiniLab3Template();
    
    juce::ValueTree toValueTree() const;
    void fromValueTree(const juce::ValueTree& tree);
    
private:
    mutable juce::CriticalSection mappingLock;
    std::vector<MIDIMapping> mappings;
    juce::String learningParameterID;
    
    MIDIMapping* findMappingByCC(int ccNumber, int channel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIControllerMap)
};
