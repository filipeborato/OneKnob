# OneKnob - JUCE One-Knob Plugin Template

A minimal, production-ready template for building one-knob audio plugins with JUCE. It ships with a single parameter, a custom rotary knob animation (10 PNG frames), and a stylized wooden panel UI. Use this as the baseline for your own family of simple one-control VST3 plugins.

## Features
- One parameter: Gain (0.0 - 1.0), linear taper.
- AudioProcessorValueTreeState for parameter/state management and host automation.
- Persistent state save/restore (XML via AVTS).
- Custom LookAndFeel with animated rotary knob (10 frames in `BinaryData`).
- Clean, centered UI with wooden frame styling.
- JUCE project configured via `OneKnob.jucer` for cross-platform builds.

## Repository Layout
- `Source/PluginProcessor.*`: Audio processing, parameters, state.
- `Source/PluginEditor.*`: UI, custom rotary knob, frame drawing, attachments.
- `JuceLibraryCode/`: JUCE auto-generated files (do not edit manually).
- `Builds/`: Exporter-specific build projects and outputs.
- `OneKnob.jucer`: Projucer project file.

## Requirements
- JUCE 7.x (Projucer and modules).
- A C++17 toolchain and IDE:
  - Windows: Visual Studio 2022 (recommended).
  - macOS: Xcode 14+.
  - Linux: Makefiles or CLion + appropriate toolchain.
- Optional: A plugin host for testing (e.g., your DAW or JUCE AudioPluginHost).

## Build (VST3 / Standalone)
1. Open `OneKnob.jucer` in Projucer.
2. In Project Settings:
   - Set your JUCE modules path if prompted.
   - Under Plugin Formats, enable at least `VST3` and/or `Standalone`.
3. In Exporters, add your platform exporter (Visual Studio, Xcode, Linux Makefiles).
4. Save the project, then click "Open in IDE".
5. Build the target in your IDE.

Outputs are placed under `Builds/<Exporter>/<Config>/`. The plugin binary is typically:
- Windows VST3: `Builds/VisualStudio*/x64/Release/<ProjectName>.vst3`
- macOS VST3: `Builds/MacOSX/build/Release/<ProjectName>.vst3`
- Standalone app: Same tree under the exporter’s output directory.

## Install (VST3)
- Windows (system): `C:\\Program Files\\Common Files\\VST3`
- Windows (user): `%LOCALAPPDATA%\\Programs\\Common\\VST3`
- macOS (system): `/Library/Audio/Plug-Ins/VST3`
- macOS (user): `~/Library/Audio/Plug-Ins/VST3`

Copy the built `.vst3` bundle to one of the above paths, then rescan in your host.

## Usage
Insert the plugin on an audio track and turn the single knob:
- Gain: 0.0 (silence) to 1.0 (unity). This template applies linear gain to all channels. Adjust the parameter mapping or range to suit your plugin’s intent.

## Customize This Template
Use these steps to turn this into your own one-knob plugin.

- Project naming:
  - In Projucer, change `Project Name` and the `Plugin Name`/`Plugin Code`/`Company` fields (Project Settings -> Plugin Characteristics).
  - Ensure `Plugin Code` (4 letters) and `Plugin Identifier` are unique across your plugins to avoid host collisions.

- Parameter(s):
  - Edit `Source/PluginProcessor.*`:
    - Change the parameter ID/name/range in `createParameterLayout()`.
    - Optionally apply non-linear mapping (e.g., `NormalisableRange` with skew) for more natural control.
  - Keep the ID stable to preserve automation in user projects.

- DSP behavior:
  - Modify `processBlock(...)` to implement your algorithm (saturation, filter, wet/dry mix, etc.).

- UI:
  - `Source/PluginEditor.*` holds the rotary slider and custom look.
  - Title text: update the string drawn in `drawTitle(...)`.
  - Replace the animated knob frames:
    - Add your PNGs to Projucer (they become `BinaryData::<name>_png`).
    - Update `loadKnobFrames()` order to map frames to rotation.
    - Keep all frames square and same dimensions for best results.
  - Adjust the rotation arc with `setRotaryParameters(startAngle, endAngle, true)`.

- Formats and categories:
  - In Projucer -> Plugin Characteristics, set categories (e.g., Dynamics, EQ) and enable/disable AU, LV2, Standalone as needed (VST2 is deprecated and generally unavailable).

- Versioning and metadata:
  - Update version, company, and website fields in Projucer for proper bundle metadata.

## Code Entry Points
- `OneKnobAudioProcessor::createParameterLayout()`: Define parameters.
- `OneKnobAudioProcessor::processBlock(...)`: Apply DSP using parameter values.
- `OneKnobAudioProcessorEditor`: Construct UI, attach controls to parameters.

## Testing Tips
- Use JUCE's AudioPluginHost or your DAW to validate parameter automation and state save/restore.
- Verify unique plugin IDs by loading multiple one-knob variants together in a session.
- On Windows, check both Debug and Release builds due to CRT differences.

## Notes
- This repo includes JUCE-generated files under `JuceLibraryCode/`. Manage them via Projucer rather than editing by hand.
- No license is provided here; add your own `LICENSE` if you plan to distribute.

## Roadmap (suggested)
- Add a parameter readout/tooltip under the knob.
- Optional non-linear tapers (log/exp) for musical response.
- Preset support and parameter smoothing.
- CI builds for Windows/macOS and artifact upload.

---
Built with JUCE.
