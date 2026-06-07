# RhythmicSpace — Cloud Agent Instructions

## Project

JUCE 8 audio plugin (AU, VST3, Standalone). Step-sequencer multi-effect processor.

## Build

Requires **macOS + Xcode**. Cannot compile on Linux cloud VMs.

```bash
# On macOS after opening RhythmicSpace.jucer in Projucer:
# Save and Open in IDE → Release build (AU + VST3 targets)
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
