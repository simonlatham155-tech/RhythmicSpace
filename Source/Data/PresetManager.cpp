#include "PresetManager.h"

//==============================================================================
PresetManager::PresetManager()
{
    createFactoryPresets();
    loadUserPresets();
}

PresetManager::~PresetManager()
{
}

//==============================================================================
const Preset* PresetManager::getPreset(int index) const
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return &presets[index];
    return nullptr;
}

juce::File PresetManager::getUserPresetsDirectory() const
{
    auto appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    auto presetDir = appData.getChildFile("LATHAMAUDIO").getChildFile("RhythmicSpace").getChildFile("Presets");
    
    if (!presetDir.exists())
        presetDir.createDirectory();
    
    return presetDir;
}

void PresetManager::saveUserPreset(const Preset& preset)
{
    // Add to memory
    presets.push_back(preset);
    
    // Save to file
    auto presetFile = getUserPresetsDirectory().getChildFile(preset.name + ".rsp");
    
    // Create XML
    juce::XmlElement xml("RhythmicSpacePreset");
    xml.setAttribute("name", preset.name);
    xml.setAttribute("category", preset.category);
    xml.setAttribute("bpm", preset.bpm);
    
    // Save effect parameters
    auto* effectParams = xml.createNewChildElement("EffectParameters");
    effectParams->setAttribute("filterCutoff", preset.filterCutoff);
    effectParams->setAttribute("filterResonance", preset.filterResonance);
    effectParams->setAttribute("filterType", preset.filterType);
    effectParams->setAttribute("filterMix", preset.filterMix);
    effectParams->setAttribute("delayTime", preset.delayTime);
    effectParams->setAttribute("delayFeedback", preset.delayFeedback);
    effectParams->setAttribute("delayMix", preset.delayMix);
    effectParams->setAttribute("reverbSize", preset.reverbSize);
    effectParams->setAttribute("reverbDamping", preset.reverbDamping);
    effectParams->setAttribute("reverbMix", preset.reverbMix);
    
    // Save step sequences
    auto saveSteps = [&](const juce::String& name, const std::array<float, 16>& steps)
    {
        auto* stepsElement = xml.createNewChildElement(name);
        juce::String stepsStr;
        for (int i = 0; i < 16; ++i)
        {
            if (i > 0) stepsStr += ",";
            stepsStr += juce::String(steps[i]);
        }
        stepsElement->setAttribute("values", stepsStr);
    };
    
    saveSteps("FilterSteps", preset.filterSteps);
    saveSteps("PanSteps", preset.panSteps);
    saveSteps("DelaySteps", preset.delaySteps);
    saveSteps("ReverbSteps", preset.reverbSteps);
    saveSteps("VolumeSteps", preset.volumeSteps);
    
    // Write to file
    xml.writeTo(presetFile);
}

void PresetManager::deleteUserPreset(int index)
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
    {
        auto& preset = presets[index];
        
        // Delete file
        auto presetFile = getUserPresetsDirectory().getChildFile(preset.name + ".rsp");
        if (presetFile.exists())
            presetFile.deleteFile();
        
        // Remove from memory
        presets.erase(presets.begin() + index);
    }
}

void PresetManager::loadUserPresets()
{
    auto presetDir = getUserPresetsDirectory();
    auto presetFiles = presetDir.findChildFiles(juce::File::findFiles, false, "*.rsp");
    
    for (const auto& file : presetFiles)
    {
        auto xml = juce::parseXML(file);
        if (xml != nullptr && xml->hasTagName("RhythmicSpacePreset"))
        {
            Preset preset;
            preset.name = xml->getStringAttribute("name");
            preset.category = xml->getStringAttribute("category", "User");
            preset.bpm = xml->getDoubleAttribute("bpm", 120.0);
            
            // Load effect parameters
            if (auto* effectParams = xml->getChildByName("EffectParameters"))
            {
                preset.filterCutoff = effectParams->getDoubleAttribute("filterCutoff", 1000.0);
                preset.filterResonance = effectParams->getDoubleAttribute("filterResonance", 0.7);
                preset.filterType = effectParams->getIntAttribute("filterType", 0);
                preset.filterMix = effectParams->getDoubleAttribute("filterMix", 100.0);
                preset.delayTime = effectParams->getDoubleAttribute("delayTime", 0.5);
                preset.delayFeedback = effectParams->getDoubleAttribute("delayFeedback", 0.5);
                preset.delayMix = effectParams->getDoubleAttribute("delayMix", 50.0);
                preset.reverbSize = effectParams->getDoubleAttribute("reverbSize", 0.5);
                preset.reverbDamping = effectParams->getDoubleAttribute("reverbDamping", 0.5);
                preset.reverbMix = effectParams->getDoubleAttribute("reverbMix", 30.0);
            }
            
            // Load step sequences
            auto loadSteps = [&](const juce::String& name, std::array<float, 16>& steps)
            {
                if (auto* stepsElement = xml->getChildByName(name))
                {
                    auto stepsStr = stepsElement->getStringAttribute("values");
                    auto tokens = juce::StringArray::fromTokens(stepsStr, ",", "");
                    for (int i = 0; i < 16 && i < tokens.size(); ++i)
                        steps[i] = tokens[i].getFloatValue();
                }
            };
            
            loadSteps("FilterSteps", preset.filterSteps);
            loadSteps("PanSteps", preset.panSteps);
            loadSteps("DelaySteps", preset.delaySteps);
            loadSteps("ReverbSteps", preset.reverbSteps);
            loadSteps("VolumeSteps", preset.volumeSteps);
            
            presets.push_back(preset);
        }
    }
}

Preset PresetManager::createPresetFromCurrentState(const juce::String& name,
                                                    const juce::String& category,
                                                    const StepSequencer& sequencer,
                                                    const juce::AudioProcessorValueTreeState& parameters)
{
    Preset preset;
    preset.name = name;
    preset.category = category;
    preset.bpm = sequencer.getBPM();
    
    // Get effect parameters
    preset.filterCutoff = parameters.getRawParameterValue("filterCutoff")->load();
    preset.filterResonance = parameters.getRawParameterValue("filterResonance")->load();
    preset.filterType = parameters.getRawParameterValue("filterType")->load();
    preset.filterMix = parameters.getRawParameterValue("filterMix")->load();
    
    preset.delayTime = parameters.getRawParameterValue("delayTime")->load();
    preset.delayFeedback = parameters.getRawParameterValue("delayFeedback")->load();
    preset.delayMix = parameters.getRawParameterValue("delayMix")->load();
    
    preset.reverbSize = parameters.getRawParameterValue("reverbSize")->load();
    preset.reverbDamping = parameters.getRawParameterValue("reverbDamping")->load();
    preset.reverbMix = parameters.getRawParameterValue("reverbMix")->load();
    
    // Get step sequences
    preset.filterSteps = sequencer.getSteps(StepSequencer::FilterParam);
    preset.panSteps = sequencer.getSteps(StepSequencer::PanParam);
    preset.delaySteps = sequencer.getSteps(StepSequencer::DelayParam);
    preset.reverbSteps = sequencer.getSteps(StepSequencer::ReverbParam);
    preset.volumeSteps = sequencer.getSteps(StepSequencer::VolumeParam);
    
    return preset;
}

juce::StringArray PresetManager::getCategories() const
{
    juce::StringArray categories;
    for (const auto& preset : presets)
    {
        if (!categories.contains(preset.category))
            categories.add(preset.category);
    }
    return categories;
}

juce::Array<int> PresetManager::getPresetsInCategory(const juce::String& category) const
{
    juce::Array<int> indices;
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
    {
        if (presets[i].category == category)
            indices.add(i);
    }
    return indices;
}

//==============================================================================
void PresetManager::createFactoryPresets()
{
    // Preset 1: Filter Wobble
    {
        Preset preset;
        preset.name = "Filter Wobble";
        preset.category = "Rhythmic";
        preset.bpm = 120.0;
        preset.filterSteps = convertToNormalized({20, 80, 20, 80, 30, 90, 30, 90, 20, 80, 20, 80, 30, 90, 30, 90});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
        preset.reverbSteps = convertToNormalized({20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20});
        preset.volumeSteps = convertToNormalized({80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80});
        presets.push_back(preset);
    }
    
    // Preset 2: Stutter Gate
    {
        Preset preset;
        preset.name = "Stutter Gate";
        preset.category = "Rhythmic";
        preset.bpm = 128.0;
        preset.filterSteps = convertToNormalized({70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({30, 0, 30, 0, 50, 0, 30, 0, 30, 0, 30, 0, 50, 0, 30, 0});
        preset.reverbSteps = convertToNormalized({15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15});
        preset.volumeSteps = convertToNormalized({100, 0, 80, 0, 100, 0, 60, 0, 100, 0, 80, 0, 100, 0, 40, 20});
        presets.push_back(preset);
    }
    
    // Preset 3: Ping Pong Delay
    {
        Preset preset;
        preset.name = "Ping Pong Delay";
        preset.category = "Delay";
        preset.bpm = 115.0;
        preset.filterSteps = convertToNormalized({80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80});
        preset.panSteps = convertToNormalized({0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100});
        preset.delaySteps = convertToNormalized({70, 60, 50, 40, 70, 60, 50, 40, 70, 60, 50, 40, 70, 60, 50, 40});
        preset.reverbSteps = convertToNormalized({35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35});
        preset.volumeSteps = convertToNormalized({75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75});
        presets.push_back(preset);
    }
    
    // Preset 4: Space Reverb
    {
        Preset preset;
        preset.name = "Space Reverb";
        preset.category = "Ambient";
        preset.bpm = 90.0;
        preset.filterSteps = convertToNormalized({40, 45, 50, 55, 60, 65, 70, 75, 80, 75, 70, 65, 60, 55, 50, 45});
        preset.panSteps = convertToNormalized({30, 35, 40, 45, 50, 55, 60, 65, 70, 65, 60, 55, 50, 45, 40, 35});
        preset.delaySteps = convertToNormalized({25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25});
        preset.reverbSteps = convertToNormalized({60, 65, 70, 75, 80, 85, 90, 95, 90, 85, 80, 75, 70, 65, 60, 55});
        preset.volumeSteps = convertToNormalized({70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70});
        presets.push_back(preset);
    }
    
    // Preset 5: Tremolo
    {
        Preset preset;
        preset.name = "Tremolo";
        preset.category = "Modulation";
        preset.bpm = 120.0;
        preset.filterSteps = convertToNormalized({75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10});
        preset.reverbSteps = convertToNormalized({25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25});
        preset.volumeSteps = convertToNormalized({100, 80, 60, 40, 30, 40, 60, 80, 100, 80, 60, 40, 30, 40, 60, 80});
        presets.push_back(preset);
    }
    
    // Preset 6: Auto Pan
    {
        Preset preset;
        preset.name = "Auto Pan";
        preset.category = "Modulation";
        preset.bpm = 110.0;
        preset.filterSteps = convertToNormalized({70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70});
        preset.panSteps = convertToNormalized({0, 10, 25, 40, 60, 75, 90, 100, 100, 90, 75, 60, 40, 25, 10, 0});
        preset.delaySteps = convertToNormalized({15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15});
        preset.reverbSteps = convertToNormalized({30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30});
        preset.volumeSteps = convertToNormalized({80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80});
        presets.push_back(preset);
    }
    
    // Preset 7: Filter Sweep
    {
        Preset preset;
        preset.name = "Filter Sweep";
        preset.category = "Filter";
        preset.bpm = 125.0;
        preset.filterSteps = convertToNormalized({10, 15, 20, 25, 30, 40, 50, 60, 70, 75, 80, 85, 90, 95, 98, 100});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20});
        preset.reverbSteps = convertToNormalized({20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95});
        preset.volumeSteps = convertToNormalized({75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75});
        presets.push_back(preset);
    }
    
    // Preset 8: Glitch Stutter
    {
        Preset preset;
        preset.name = "Glitch Stutter";
        preset.category = "Experimental";
        preset.bpm = 140.0;
        preset.filterSteps = convertToNormalized({90, 30, 70, 20, 80, 40, 60, 10, 85, 25, 75, 35, 65, 15, 95, 50});
        preset.panSteps = convertToNormalized({100, 0, 50, 100, 0, 50, 100, 0, 50, 100, 0, 50, 100, 0, 50, 0});
        preset.delaySteps = convertToNormalized({80, 0, 60, 0, 70, 0, 50, 0, 90, 0, 40, 0, 75, 0, 55, 0});
        preset.reverbSteps = convertToNormalized({10, 40, 20, 50, 15, 35, 25, 45, 30, 60, 20, 40, 15, 50, 25, 35});
        preset.volumeSteps = convertToNormalized({100, 50, 80, 30, 90, 40, 70, 20, 100, 60, 85, 35, 95, 45, 75, 25});
        presets.push_back(preset);
    }
    
    // Preset 9: Drum Bounce
    {
        Preset preset;
        preset.name = "Drum Bounce";
        preset.category = "Rhythmic";
        preset.bpm = 128.0;
        preset.filterSteps = convertToNormalized({80, 40, 60, 30, 80, 40, 60, 30, 80, 40, 60, 30, 80, 40, 60, 30});
        preset.panSteps = convertToNormalized({40, 60, 60, 40, 40, 60, 60, 40, 40, 60, 60, 40, 40, 60, 60, 40});
        preset.delaySteps = convertToNormalized({0, 0, 0, 40, 0, 0, 0, 50, 0, 0, 0, 40, 0, 0, 0, 60});
        preset.reverbSteps = convertToNormalized({15, 15, 15, 30, 15, 15, 15, 35, 15, 15, 15, 30, 15, 15, 15, 40});
        preset.volumeSteps = convertToNormalized({100, 70, 85, 60, 100, 70, 85, 55, 100, 70, 85, 60, 100, 70, 85, 50});
        presets.push_back(preset);
    }
    
    // Preset 10: Subtle Movement
    {
        Preset preset;
        preset.name = "Subtle Movement";
        preset.category = "Ambient";
        preset.bpm = 95.0;
        preset.filterSteps = convertToNormalized({55, 58, 60, 63, 65, 63, 60, 58, 55, 58, 60, 63, 65, 63, 60, 58});
        preset.panSteps = convertToNormalized({45, 48, 50, 52, 55, 52, 50, 48, 45, 48, 50, 52, 55, 52, 50, 48});
        preset.delaySteps = convertToNormalized({12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12});
        preset.reverbSteps = convertToNormalized({40, 42, 45, 47, 50, 47, 45, 42, 40, 42, 45, 47, 50, 47, 45, 42});
        preset.volumeSteps = convertToNormalized({75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75});
        presets.push_back(preset);
    }
    
    // Preset 11: Helicopter
    {
        Preset preset;
        preset.name = "Helicopter";
        preset.category = "Modulation";
        preset.bpm = 118.0;
        preset.filterSteps = convertToNormalized({65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65});
        preset.panSteps = convertToNormalized({0, 25, 50, 75, 100, 75, 50, 25, 0, 25, 50, 75, 100, 75, 50, 25});
        preset.delaySteps = convertToNormalized({18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18});
        preset.reverbSteps = convertToNormalized({40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40});
        preset.volumeSteps = convertToNormalized({90, 70, 50, 70, 90, 70, 50, 70, 90, 70, 50, 70, 90, 70, 50, 70});
        presets.push_back(preset);
    }
    
    // Preset 12: Reverse Swell
    {
        Preset preset;
        preset.name = "Reverse Swell";
        preset.category = "Build-Up";
        preset.bpm = 105.0;
        preset.filterSteps = convertToNormalized({100, 95, 90, 85, 75, 65, 55, 45, 35, 28, 22, 17, 13, 10, 7, 5});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25});
        preset.reverbSteps = convertToNormalized({90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15});
        preset.volumeSteps = convertToNormalized({100, 95, 90, 85, 80, 70, 60, 50, 40, 32, 25, 19, 14, 10, 6, 3});
        presets.push_back(preset);
    }
    
    // Preset 13: Triplet Groove
    {
        Preset preset;
        preset.name = "Triplet Groove";
        preset.category = "Rhythmic";
        preset.bpm = 133.0;
        preset.filterSteps = convertToNormalized({70, 50, 80, 60, 50, 85, 65, 50, 75, 55, 50, 90, 70, 50, 80, 60});
        preset.panSteps = convertToNormalized({50, 65, 35, 50, 65, 35, 50, 65, 35, 50, 65, 35, 50, 65, 35, 50});
        preset.delaySteps = convertToNormalized({60, 20, 40, 70, 20, 50, 60, 20, 45, 75, 20, 55, 65, 20, 50, 70});
        preset.reverbSteps = convertToNormalized({25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25});
        preset.volumeSteps = convertToNormalized({90, 70, 85, 95, 70, 90, 90, 70, 80, 95, 70, 85, 90, 70, 80, 95});
        presets.push_back(preset);
    }
    
    // Preset 14: Phaser
    {
        Preset preset;
        preset.name = "Phaser";
        preset.category = "Filter";
        preset.bpm = 122.0;
        preset.filterSteps = convertToNormalized({30, 40, 50, 60, 70, 80, 90, 100, 90, 80, 70, 60, 50, 40, 30, 20});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8});
        preset.reverbSteps = convertToNormalized({18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18});
        preset.volumeSteps = convertToNormalized({80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80});
        presets.push_back(preset);
    }
    
    // Preset 15: Sidechain Pump
    {
        Preset preset;
        preset.name = "Sidechain Pump";
        preset.category = "Rhythmic";
        preset.bpm = 128.0;
        preset.filterSteps = convertToNormalized({50, 55, 60, 65, 50, 55, 60, 65, 50, 55, 60, 65, 50, 55, 60, 65});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({0, 10, 20, 15, 0, 10, 20, 15, 0, 10, 20, 15, 0, 10, 20, 15});
        preset.reverbSteps = convertToNormalized({15, 20, 25, 20, 15, 20, 25, 20, 15, 20, 25, 20, 15, 20, 25, 20});
        preset.volumeSteps = convertToNormalized({20, 40, 60, 80, 20, 40, 60, 80, 20, 40, 60, 80, 20, 40, 60, 80});
        presets.push_back(preset);
    }
    
    // Preset 16: Dramatic Riser
    {
        Preset preset;
        preset.name = "Dramatic Riser";
        preset.category = "Build-Up";
        preset.bpm = 100.0;
        preset.filterSteps = convertToNormalized({5, 10, 15, 22, 30, 38, 47, 56, 65, 73, 80, 86, 91, 95, 98, 100});
        preset.panSteps = convertToNormalized({50, 48, 52, 47, 53, 46, 54, 45, 55, 44, 56, 43, 57, 42, 58, 50});
        preset.delaySteps = convertToNormalized({10, 12, 15, 18, 22, 26, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75});
        preset.reverbSteps = convertToNormalized({30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 93, 96, 100});
        preset.volumeSteps = convertToNormalized({60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 85, 88, 92, 96});
        presets.push_back(preset);
    }
    
    // Preset 17: Stereo Width
    {
        Preset preset;
        preset.name = "Stereo Width";
        preset.category = "Delay";
        preset.bpm = 112.0;
        preset.filterSteps = convertToNormalized({75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75});
        preset.panSteps = convertToNormalized({0, 15, 100, 85, 0, 20, 100, 80, 0, 25, 100, 75, 0, 30, 100, 70});
        preset.delaySteps = convertToNormalized({35, 30, 35, 30, 35, 30, 35, 30, 35, 30, 35, 30, 35, 30, 35, 30});
        preset.reverbSteps = convertToNormalized({45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45});
        preset.volumeSteps = convertToNormalized({78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78});
        presets.push_back(preset);
    }
    
    // Preset 18: Lo-Fi Crush
    {
        Preset preset;
        preset.name = "Lo-Fi Crush";
        preset.category = "Experimental";
        preset.bpm = 90.0;
        preset.filterSteps = convertToNormalized({25, 30, 20, 35, 15, 40, 25, 30, 20, 35, 18, 42, 22, 38, 16, 45});
        preset.panSteps = convertToNormalized({30, 70, 40, 60, 35, 65, 45, 55, 30, 70, 40, 60, 35, 65, 45, 55});
        preset.delaySteps = convertToNormalized({45, 30, 50, 25, 55, 35, 48, 32, 52, 28, 58, 38, 46, 34, 54, 40});
        preset.reverbSteps = convertToNormalized({25, 30, 28, 32, 26, 34, 27, 33, 29, 31, 25, 35, 28, 32, 26, 30});
        preset.volumeSteps = convertToNormalized({85, 75, 90, 70, 88, 72, 86, 78, 82, 76, 84, 74, 87, 73, 89, 71});
        presets.push_back(preset);
    }
    
    // Preset 19: Eighth Note Delay
    {
        Preset preset;
        preset.name = "Eighth Note Delay";
        preset.category = "Delay";
        preset.bpm = 120.0;
        preset.filterSteps = convertToNormalized({72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72});
        preset.panSteps = convertToNormalized({45, 55, 45, 55, 45, 55, 45, 55, 45, 55, 45, 55, 45, 55, 45, 55});
        preset.delaySteps = convertToNormalized({0, 75, 0, 70, 0, 75, 0, 68, 0, 75, 0, 72, 0, 75, 0, 70});
        preset.reverbSteps = convertToNormalized({38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38});
        preset.volumeSteps = convertToNormalized({85, 80, 85, 78, 85, 82, 85, 76, 85, 80, 85, 78, 85, 82, 85, 76});
        presets.push_back(preset);
    }
    
    // Preset 20: Bass Wobble (Slow)
    {
        Preset preset;
        preset.name = "Bass Wobble (Slow)";
        preset.category = "Bass";
        preset.bpm = 85.0;
        preset.filterSteps = convertToNormalized({15, 15, 15, 15, 85, 85, 85, 85, 15, 15, 15, 15, 85, 85, 85, 85});
        preset.panSteps = convertToNormalized({50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50});
        preset.delaySteps = convertToNormalized({5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5});
        preset.reverbSteps = convertToNormalized({12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12});
        preset.volumeSteps = convertToNormalized({90, 88, 85, 83, 90, 88, 85, 83, 90, 88, 85, 83, 90, 88, 85, 83});
        presets.push_back(preset);
    }
    
    // Preset 21: Arpeggiated Pan
    {
        Preset preset;
        preset.name = "Arpeggiated Pan";
        preset.category = "Modulation";
        preset.bpm = 130.0;
        preset.filterSteps = convertToNormalized({68, 70, 72, 75, 68, 70, 72, 75, 68, 70, 72, 75, 68, 70, 72, 75});
        preset.panSteps = convertToNormalized({10, 90, 30, 70, 20, 80, 40, 60, 10, 90, 30, 70, 20, 80, 40, 60});
        preset.delaySteps = convertToNormalized({25, 22, 28, 20, 26, 23, 27, 21, 24, 22, 29, 20, 25, 23, 28, 21});
        preset.reverbSteps = convertToNormalized({32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32});
        preset.volumeSteps = convertToNormalized({82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82});
        presets.push_back(preset);
    }
    
    // Preset 22: Rhythmic Chop
    {
        Preset preset;
        preset.name = "Rhythmic Chop";
        preset.category = "Rhythmic";
        preset.bpm = 128.0;
        preset.filterSteps = convertToNormalized({75, 75, 40, 75, 75, 35, 75, 75, 75, 75, 40, 75, 75, 30, 75, 75});
        preset.panSteps = convertToNormalized({50, 50, 65, 50, 50, 35, 50, 50, 50, 50, 65, 50, 50, 35, 50, 50});
        preset.delaySteps = convertToNormalized({0, 0, 0, 45, 0, 0, 0, 55, 0, 0, 0, 48, 0, 0, 0, 60});
        preset.reverbSteps = convertToNormalized({8, 8, 35, 8, 8, 40, 8, 8, 8, 8, 38, 8, 8, 42, 8, 8});
        preset.volumeSteps = convertToNormalized({100, 95, 0, 90, 100, 0, 85, 95, 100, 92, 0, 88, 100, 0, 82, 94});
        presets.push_back(preset);
    }
    
    // Preset 23: Ambient Wash
    {
        Preset preset;
        preset.name = "Ambient Wash";
        preset.category = "Ambient";
        preset.bpm = 88.0;
        preset.filterSteps = convertToNormalized({52, 54, 56, 58, 60, 62, 64, 66, 64, 62, 60, 58, 56, 54, 52, 50});
        preset.panSteps = convertToNormalized({42, 44, 46, 48, 50, 52, 54, 56, 58, 56, 54, 52, 50, 48, 46, 44});
        preset.delaySteps = convertToNormalized({38, 40, 42, 44, 46, 48, 50, 52, 50, 48, 46, 44, 42, 40, 38, 36});
        preset.reverbSteps = convertToNormalized({70, 72, 74, 76, 78, 80, 82, 85, 82, 80, 78, 76, 74, 72, 70, 68});
        preset.volumeSteps = convertToNormalized({72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72});
        presets.push_back(preset);
    }
    
    // Preset 24: Breakbeat Shuffle
    {
        Preset preset;
        preset.name = "Breakbeat Shuffle";
        preset.category = "Rhythmic";
        preset.bpm = 174.0;
        preset.filterSteps = convertToNormalized({65, 45, 70, 50, 68, 42, 75, 48, 66, 46, 72, 52, 70, 44, 78, 50});
        preset.panSteps = convertToNormalized({48, 52, 47, 53, 49, 51, 46, 54, 48, 52, 47, 53, 49, 51, 46, 54});
        preset.delaySteps = convertToNormalized({0, 25, 0, 30, 0, 28, 0, 35, 0, 26, 0, 32, 0, 29, 0, 38});
        preset.reverbSteps = convertToNormalized({18, 22, 20, 25, 19, 23, 21, 28, 18, 24, 20, 26, 19, 23, 22, 30});
        preset.volumeSteps = convertToNormalized({95, 75, 100, 68, 92, 72, 100, 65, 94, 74, 100, 70, 96, 71, 100, 66});
        presets.push_back(preset);
    }
}
