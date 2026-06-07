# AGENTS.md

## Project overview

**RhythmicSpace** is a JUCE 8.0.10 C++17 audio plugin (AU, VST3, Standalone). There is no npm/pip stack, no Docker, and no automated test or lint suite in this repository.

## Cursor Cloud specific instructions

### Linux development (Cloud Agent VM)

This repo targets macOS in the README, but it also exports a **Linux Makefile** via Projucer. On Linux:

1. **System packages** (one-time VM image setup; not in the update script): ALSA, JACK, Freetype, X11, WebKit2GTK, and related JUCE Linux deps via `apt`.
2. **JUCE 8.0.10** lives at `/workspace/JUCE` (cloned separately; gitignored from the app repo).
3. **Projucer** must be built once from `JUCE/extras/Projucer/Builds/LinuxMakefile` and its global module path set:
   ```bash
   JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer \
     --set-global-search-path linux defaultJuceModulePath /workspace/JUCE/modules
   ```
   Projucer requires `~/.config/Projucer/Projucer.settings` to exist (launch Projucer once, or run it briefly) before `--set-global-search-path` works.
4. **Generate build files** (writes `Builds/` and `JuceLibraryCode/`, both gitignored):
   ```bash
   JUCE/extras/Projucer/Builds/LinuxMakefile/build/Projucer --resave RhythmicSpace.jucer
   ```
5. **Build**:
   ```bash
   cd Builds/LinuxMakefile
   make CONFIG=Debug -j"$(nproc)"
   ```
6. **Run standalone** (requires X11 `DISPLAY`, e.g. `:1` in Cloud Agent):
   ```bash
   cd Builds/LinuxMakefile/build
   DISPLAY=:1 ./RhythmicSpace
   ```

### Build targets

| Target | Output |
|--------|--------|
| Standalone | `Builds/LinuxMakefile/build/RhythmicSpace` |
| VST3 | `Builds/LinuxMakefile/build/RhythmicSpace.vst3` (also copied to `~/.vst3`) |

### Lint / tests

There is no configured linter or test runner. **Compile success** (`make CONFIG=Debug`) is the primary verification step. Expect deprecation warnings from JUCE `Font` constructors; they do not block the build.

### macOS / Windows

Follow `README.md`: install JUCE 8.0.10, open `RhythmicSpace.jucer` in Projucer, save to Xcode or Visual Studio 2022, then build the desired target (AU, VST3, or Standalone).

### Static docs site

`docs/index.html` is a GitHub Pages download page only; no build step required.
