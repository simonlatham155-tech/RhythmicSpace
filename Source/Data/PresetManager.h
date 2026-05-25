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
    
    // Preset access
    int getNumPresets() const { return static_cast<int>(presets.size()); }
    const Preset* getPreset(int index) const;
    
    // Preset management
    void saveUserPreset(const Preset& preset);
    void deleteUserPreset(int index);
    void loadUserPresets();
    
    // Get current state as preset
    Preset createPresetFromCurrentState(const juce::String& name, 
                                        const juce::String& category,
                                        const StepSequencer& sequencer,
                                        const juce::AudioProcessorValueTreeState& parameters);
    
    // Categories
    juce::StringArray getCategories() const;
    juce::Array<int> getPresetsInCategory(const juce::String& category) const;
    
    // User preset directory
    juce::File getUserPresetsDirectory() const;

private:
    std::vector<Preset> presets;
    
    void createFactoryPresets();
    
    // Helper to convert 0-100 arrays to 0.0-1.0
    static std::array<float, 16> convertToNormalized(const std::array<float, 16>& input)
    {
        std::array<float, 16> output;
        for (int i = 0; i < 16; ++i)
            output[i] = input[i] / 100.0f;
        return output;
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
