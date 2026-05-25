#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * MIDI Controller Mapping System
 * 
 * Handles MIDI CC mapping to plugin parameters with MIDI Learn functionality
 */
class MIDIControllerMap
{
public:
    //==========================================================================
    // MIDI Mapping Structure
    struct MIDIMapping
    {
        int ccNumber = -1;           // MIDI CC number (0-127)
        int channel = -1;            // MIDI channel (1-16, or -1 for omni)
        juce::String parameterID;    // Parameter ID to control
        float minValue = 0.0f;       // Min value range
        float maxValue = 1.0f;       // Max value range
        bool isLearning = false;     // Currently in learn mode
        
        bool isValid() const { return ccNumber >= 0 && ccNumber <= 127; }
    };
    
    //==========================================================================
    // Controller Templates
    enum class ControllerTemplate
    {
        None,
        ArturiaMiniLab3,
        LaunchpadMini,
        CustomMapping
    };
    
    //==========================================================================
    MIDIControllerMap();
    ~MIDIControllerMap() = default;
    
    // MIDI Learn
    void startLearning(const juce::String& parameterID);
    void stopLearning();
    bool isLearning() const { return learningParameterID.isNotEmpty(); }
    juce::String getLearningParameterID() const { return learningParameterID; }
    
    // Handle incoming MIDI
    bool processMIDIMessage(const juce::MidiMessage& message, 
                           std::function<void(const juce::String&, float)> parameterCallback);
    
    // Mapping management
    void addMapping(int ccNumber, int channel, const juce::String& parameterID,
                   float minValue = 0.0f, float maxValue = 1.0f);
    void removeMapping(const juce::String& parameterID);
    void clearAllMappings();
    MIDIMapping* getMapping(const juce::String& parameterID);
    const MIDIMapping* getMapping(const juce::String& parameterID) const;
    
    // Get all mappings
    const std::vector<MIDIMapping>& getAllMappings() const { return mappings; }
    
    // Controller templates
    void loadTemplate(ControllerTemplate templateType);
    void loadArturiaMiniLab3Template();
    
    // Persistence
    juce::ValueTree toValueTree() const;
    void fromValueTree(const juce::ValueTree& tree);
    
private:
    std::vector<MIDIMapping> mappings;
    juce::String learningParameterID;
    
    // Find mapping by CC number
    MIDIMapping* findMappingByCC(int ccNumber, int channel);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIControllerMap)
};
