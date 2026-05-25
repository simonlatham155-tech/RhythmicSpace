# LATHAMAUDIO: RhythmicSpace

**Step Sequencer Multi-Effect Processor**

A JUCE-based audio plugin that combines a step sequencer with five independent effect lanes for rhythmic audio processing.

## Features

- **5 Effect Modules:** Filter, Delay, Reverb, Pan, Volume — each with its own step sequencer lane
- **MIDI Learn:** Right-click any knob to assign a MIDI controller
- **15 Factory Presets:** Modulation, Auto Pan, Filter Sweep, Experimental, Drum Bounce, Helicopter, Build-Up, Stereo Wobble, Lo-Fi Crush, Ambient, FilterSteps, PanSteps, DelaySteps, ReverbSteps, VolumeSteps
- **Preset Browser:** Save, load, and delete custom presets
- **Transport Sync:** Lock to host tempo
- **Level Metering:** Real-time input/output monitoring

## Plugin Formats

| Format | Status |
|--------|--------|
| Audio Unit (AU) | ✅ macOS |
| VST3 | ✅ macOS / Windows |
| Standalone | ✅ macOS |

## Parameters

| Module | Parameters |
|--------|-----------|
| **Filter** | Cutoff, Resonance, Wobble, Mix |
| **Delay** | Time, Feedback, Mix |
| **Reverb** | Amount, Damping, Size, Mix |
| **Pan** | Rate, Width, Position |
| **Volume** | Amount, Gate |

## Download

Visit the [download page](https://simonlatham155-tech.github.io/RhythmicSpace/) for installers.

## Building from Source

### Requirements

- macOS 10.13+
- Xcode 14+
- [JUCE 8.0.10](https://juce.com/download/)

### Steps

1. Install JUCE and open Projucer
2. Open `RhythmicSpace.jucer`
3. Set your JUCE module path (usually `/Applications/JUCE/modules`)
4. Click "Save and Open in IDE"
5. In Xcode, select the target:
   - `RhythmicSpace - AU` for Audio Unit
   - `RhythmicSpace - VST3` for VST3
   - `RhythmicSpace - Standalone Plugin` for standalone app
6. Build with `Cmd+B` (use Release configuration for distribution)

### Install Locations

- **AU:** `~/Library/Audio/Plug-Ins/Components/RhythmicSpace.component`
- **VST3:** `~/Library/Audio/Plug-Ins/VST3/RhythmicSpace.vst3`

## Technical Details

- **Bundle ID:** `com.lathamaudio.rhythmicspace`
- **Manufacturer Code:** `Ltha`
- **Plugin Code:** `Rysp`
- **JUCE Version:** 8.0.10
- **C++ Standard:** C++17

## License

© 2025 LATHAMAUDIO. All rights reserved.
