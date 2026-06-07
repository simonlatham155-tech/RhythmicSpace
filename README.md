# LATHAMAUDIO: RhythmicSpace

**Step Sequencer Multi-Effect Processor**

A JUCE-based audio plugin that combines a step sequencer with five independent effect lanes for rhythmic audio processing.

## Features

- **5 Effect Modules:** Filter, Delay, Reverb, Pan, Volume — each with its own step sequencer lane
- **MIDI Learn:** Right-click any knob to assign a MIDI controller (mappings saved with your DAW project)
- **24 Factory Presets:** Filter Wobble, Stutter Gate, Auto Pan, Helicopter, Lo-Fi Crush, Breakbeat Shuffle, and more
- **Preset Browser:** Save, load, and delete custom presets
- **Host Sync:** Lock to DAW tempo and playhead position
- **Level Metering:** Real-time stereo input/output monitoring

## Plugin Formats

| Format | Status |
|--------|--------|
| Audio Unit (AU) | ✅ macOS |
| VST3 | ✅ macOS (Windows build planned) |
| Standalone | ✅ macOS |

## Parameters

| Module | Parameters |
|--------|-----------|
| **Filter** | Cutoff, Resonance, Type (LP/HP/BP), Mix |
| **Delay** | Time (ms), Feedback, Mix |
| **Reverb** | Size, Damping, Mix |
| **Pan** | Width, Rate (LFO) |
| **Volume** | Amount |
| **Global** | Master Volume, BPM, Transport Play/Stop |

Each effect lane also has a 16-step sequencer that modulates that effect rhythmically.

## Factory Presets

Filter Wobble · Stutter Gate · Ping Pong Delay · Space Reverb · Tremolo · Auto Pan · Filter Sweep · Glitch Stutter · Drum Bounce · Subtle Movement · Helicopter · Reverse Swell · Triplet Groove · Phaser · Sidechain Pump · Dramatic Riser · Stereo Width · Lo-Fi Crush · Eighth Note Delay · Bass Wobble (Slow) · Arpeggiated Pan · Rhythmic Chop · Ambient Wash · Breakbeat Shuffle

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
