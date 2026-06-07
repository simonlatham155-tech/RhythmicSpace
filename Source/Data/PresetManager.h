#pragma once

#include <JuceHeader.h>
#include "../DSP/StepSequencer.h"

//==============================================================================
/**
 * Manages factory and user presets
 */
class PresetManager
{
public:
    PresetManager();
    ~PresetManager();
    
    int getNumPresets() const { return static_cast<int>(presets.size()); }
    int getFactoryPresetCount() const { return factoryPresetCount; }
    const Preset* getPreset(int index) const;
    
    void saveUserPreset(const Preset& preset);
    void deleteUserPreset(int index);
    void loadUserPresets();
    
    Preset createPresetFromCurrentState(const juce::String& name, 
                                        const juce::String& category,
                                        const StepSequencer& sequencer,
                                        const juce::AudioProcessorValueTreeState& parameters);
    
    juce::StringArray getCategories() const;
    juce::Array<int> getPresetsInCategory(const juce::String& category) const;
    juce::File getUserPresetsDirectory() const;

private:
    std::vector<Preset> presets;
    int factoryPresetCount = 0;
    
    void createFactoryPresets();
    
    static std::array<float, 16> convertToNormalized(const std::array<float, 16>& input)
    {
        std::array<float, 16> output;
        for (int i = 0; i < 16; ++i)
            output[i] = input[i] / 100.0f;
        return output;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
