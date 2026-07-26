# RhythmicSpace — Cloud Agent Instructions

## Project

JUCE 8 audio plugin (AU, VST3, Standalone). Step-sequencer multi-effect processor.

## Build

**macOS + Xcode** is the canonical path for AU/VST3 release builds and DAW testing.
Linux Standalone and VST3 builds are supported for automated validation.

```bash
# On macOS after opening RhythmicSpace.jucer in Projucer:
# Save and Open in IDE → Release build (AU + VST3 targets)

# Linux after building JUCE 8.0.10 Projucer:
Projucer --set-global-search-path linux defaultJuceModulePath /path/to/JUCE/modules
Projucer --resave RhythmicSpace.jucer
make -C Builds/LinuxMakefile CONFIG=Release Standalone
make -C Builds/LinuxMakefile CONFIG=Release VST3
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
2. DAW transport with SYNC off — sequencer follows host play/stop
3. Host SYNC on — sequencer follows DAW tempo, seek and loop position
4. Filter type + preset switching works
5. AU and VST3 load without crash
