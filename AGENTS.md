# RhythmicSpace — Cloud Agent Instructions

## Project

JUCE 8 audio plugin (AU, VST3, Standalone). Step-sequencer multi-effect processor.

## Build

**macOS + Xcode** is the canonical path for AU/VST3 release builds and DAW testing.

Linux Standalone builds are supported via Projucer → Linux Makefile (see branch `cursor/dev-env-setup-5268` or cloud setup docs).

```bash
# macOS: open RhythmicSpace.jucer in Projucer → Save and Open in IDE → Release build

# Linux Standalone (after Projucer --resave):
cd Builds/LinuxMakefile && make CONFIG=Debug -j"$(nproc)"
```

## Git

- Default branch: `main`
- Feature branches: `cursor/<descriptive-name>-a9cf`
- Push with: `git push -u origin <branch-name>`

## Key paths

- `Source/PluginProcessor.cpp` — main DSP + state
- `Source/DSP/` — sequencer, effects, modulation smoother
- `Source/UI/` — editor components
- `Source/Data/` — presets, MIDI map
- `RhythmicSpace.jucer` — Projucer project

## Testing checklist (macOS DAW)

1. Save/reload project — steps, knobs, MIDI mappings persist
2. Host SYNC on — sequencer follows DAW playhead
3. Filter type + preset switching works
4. AU and VST3 load without crash
