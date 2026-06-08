# RhythmicSpace — Cloud Agent Instructions

## Project

JUCE 8 audio plugin (AU, VST3, Standalone). Step-sequencer multi-effect processor.

## Build

**Primary (distribution):** macOS 10.13+, Xcode 14+, JUCE 8.0.10 — open `RhythmicSpace.jucer` in Projucer → Save and Open in IDE → build AU / VST3 / Standalone (Release).

**Linux cloud VM (experimental):** Standalone and VST3 can be built for local validation. AU is macOS-only. No automated tests or linters in-repo.

```bash
# After VM update script (installs /opt/JUCE 8.0.10 + Projucer):
PROJUCER=/opt/JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer
$PROJUCER --resave /workspace/RhythmicSpace.jucer
make -C Builds/LinuxMakefile CONFIG=Debug Standalone   # binary: Builds/LinuxMakefile/build/RhythmicSpace
make -C Builds/LinuxMakefile CONFIG=Debug VST3           # optional
```

## Cursor Cloud specific instructions

- **JUCE location:** `/opt/JUCE` (tag `8.0.10`). Symlinks: `/JUCE` and `/workspace/JUCE` → `/opt/JUCE` (expected by `RhythmicSpace.jucer` Linux/VS module paths).
- **Projucer CLI:** `/opt/JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer`. Set global module path once per session if resave fails:  
  `$PROJUCER --set-global-search-path linux defaultJuceModulePath /opt/JUCE/modules`
- **Regenerate IDE files:** Always run `$PROJUCER --resave RhythmicSpace.jucer` after pulling if `Builds/` or `JuceLibraryCode/` are missing (both are gitignored).
- **Run Standalone headless:** `Xvfb :99 -screen 0 1280x800x24 &` then `DISPLAY=:99 Builds/LinuxMakefile/build/RhythmicSpace`. Expect the JUCE “audio input muted” banner without a real audio device.
- **No lint/test targets:** There is no CI, `clang-format`, or unit-test harness. Validate via Linux Standalone smoke run or the macOS DAW checklist below.
- **macOS remains canonical** for AU/VST3 signing, DAW host-sync, and release builds.

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
