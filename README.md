# OneKnob — one-knob JUCE tape plugin

A single-knob audio plugin built on JUCE 8. The lone `Amount` parameter drives a
coherent tape-character chain — soft saturation, gentle tilt EQ, post-saturation
HF roll-off, subtle wow & flutter, and an automatic loudness compensation so the
knob actually *colours* instead of just turning up.

| | |
|---|---|
| Format | VST3, Standalone, AU (macOS) |
| DSP | 4× oversampled `tanh` saturator + tilt EQ + wow/flutter + loudness comp |
| Latency | Reported via `setLatencySamples()` (oversampler latency, ~5–10 samples) |
| C++ | C++20 |
| JUCE | 8.0.12 (fetched automatically by CMake) |

## Repository layout

```
Source/
  PluginProcessor.{h,cpp}     # AVTS, processBlock, latency, state I/O
  PluginEditor.{h,cpp}        # modern minimal UI with amber halo
  dsp/
    AmountMapper.h            # Amount ∈ [0,1] → coherent coefficient struct
    OnePoleFilters.h          # DcBlocker, OnePoleLpf, OnePoleShelf
    Saturator.h               # tanh + asymmetric (in OS domain)
    TiltEQ.h                  # low shelf 200 Hz + high shelf 8 kHz
    WowFlutter.h              # Lagrange3 delay + dual-LFO + jitter
    LoudnessComp.h            # makeup gain with linear ramp
    TapeProcessor.{h,cpp}     # orchestrator + 4× oversampling
  lookandfeel/                # custom LookAndFeel, fonts, palette, knob component
  resources/                  # 10 PNG knob frames (animated rotary)

CMakeLists.txt                # primary build
CMakePresets.json             # ninja-debug/release/relwithdebinfo, vs2022, xcode
OneKnob.jucer                 # Projucer project (legacy, kept in sync)
```

## Requirements

- CMake **3.22+**
- A C++20 toolchain
- Internet on first configure (to fetch JUCE)
- Optional: Ninja generator, a plugin host for testing

### Per-platform prerequisites

#### Windows
- Visual Studio **2022** *(MSVC 19.3+)* or newer, with the "Desktop development with C++" workload.
- The bundled Ninja in the VS Developer Command Prompt is sufficient — no separate install needed.
- Run all CMake commands from a **VS Developer Command Prompt** (`vcvars64.bat`) so `cl.exe` and `ninja.exe` are on `PATH`.

#### macOS
- **Xcode 14+** with command-line tools: `xcode-select --install`.
- Optional: `brew install cmake ninja` if you don't already have them.

#### Linux
A recent Clang or GCC plus the JUCE GUI runtime dependencies. On Debian/Ubuntu:

```bash
sudo apt update && sudo apt install -y \
    build-essential cmake ninja-build pkg-config \
    libasound2-dev libjack-jackd2-dev \
    libcurl4-openssl-dev \
    libfreetype6-dev libfontconfig1-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxext-dev \
    libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.1-dev libgtk-3-dev
```

On Fedora the equivalents are `alsa-lib-devel`, `jack-audio-connection-kit-devel`,
`libcurl-devel`, `freetype-devel`, `fontconfig-devel`, `libX11-devel`,
`libXcomposite-devel`, `libXcursor-devel`, `libXext-devel`, `libXinerama-devel`,
`libXrandr-devel`, `libXrender-devel`, `webkit2gtk4.1-devel`, `gtk3-devel`.

Note: `libcurl` and `webkit2gtk` are required only because JUCE links them by
default; we already disable both via `JUCE_USE_CURL=0` / `JUCE_WEB_BROWSER=0`,
but the linker still needs the libraries present for the JUCE static modules
that probe them.

## Build

The repository ships a `CMakePresets.json`. JUCE is pulled in automatically via
`FetchContent` (default tag `8.0.12`, override with `-DJUCE_TAG=...`).

### Quickstart

```bash
# All platforms (Ninja, Release)
cmake --preset ninja-release
cmake --build --preset ninja-release
```

### Windows (VS Developer Command Prompt)

```bat
:: Ninja (recommended — fast, generator-agnostic)
cmake --preset ninja-release
cmake --build --preset ninja-release

:: Or Visual Studio solution
cmake --preset vs2022
cmake --build --preset vs2022-release
```

### macOS

```bash
# Ninja
cmake --preset ninja-release
cmake --build --preset ninja-release

# Or Xcode project (multi-config)
cmake --preset xcode
cmake --build --preset xcode-release
```

### Linux

```bash
cmake --preset ninja-release
cmake --build --preset ninja-release
```

### Available CMake options

Pass with `-D<name>=ON|OFF` at configure time:

| Option | Default | Description |
| --- | --- | --- |
| `ONEKNOB_BUILD_VST3` | `ON` | Build the VST3 plugin. |
| `ONEKNOB_BUILD_STANDALONE` | `ON` | Build the standalone host application. |
| `ONEKNOB_BUILD_AU` | `ON` (Apple) | Build the Audio Unit (macOS only). |
| `ONEKNOB_ENABLE_LTO` | `ON` | Link-time optimisation in Release. |
| `ONEKNOB_WARNINGS_AS_ERRORS` | `OFF` | Treat warnings as errors. |
| `ONEKNOB_COPY_PLUGIN_AFTER_BUILD` | `OFF` | Install the plugin to the system VST3/AU path after building. |
| `ONEKNOB_COMPANY_NAME` | `Borato Company` | Vendor string shown by the host. |
| `ONEKNOB_BUNDLE_ID` | `com.oneknobaudio.OneKnob` | Reverse-DNS bundle id (must be unique per plugin). |
| `JUCE_TAG` | `8.0.12` | JUCE git tag/branch/commit to fetch. |

### Build artefacts

Outputs land in `build/<preset>/OneKnob_artefacts/<Config>/`:

- VST3 bundle: `VST3/OneKnob.vst3`
- Standalone:  `Standalone/OneKnob[.exe|.app]`
- AU bundle (macOS): `AU/OneKnob.component`

## Install (VST3)

Copy the built `.vst3` bundle to one of:

- **Windows (system)**: `C:\Program Files\Common Files\VST3`
- **Windows (user)**: `%LOCALAPPDATA%\Programs\Common\VST3`
- **macOS (system)**: `/Library/Audio/Plug-Ins/VST3`
- **macOS (user)**: `~/Library/Audio/Plug-Ins/VST3`
- **Linux (user)**:   `~/.vst3`
- **Linux (system)**: `/usr/lib/vst3` or `/usr/local/lib/vst3`

Then rescan in your host. On macOS, **AU** bundles go to
`/Library/Audio/Plug-Ins/Components` (system) or
`~/Library/Audio/Plug-Ins/Components` (user). Setting
`ONEKNOB_COPY_PLUGIN_AFTER_BUILD=ON` copies the artefacts automatically.

## Build with Projucer (legacy)

`OneKnob.jucer` is kept in sync with the CMake build. Open it in Projucer, set
the JUCE modules path, save, and build in the generated IDE project. Prefer the
CMake flow for CI and reproducibility — the Projucer-generated `JuceLibraryCode/`
and `Builds/` directories are listed in `.gitignore`.

## DSP overview

`Amount` ∈ [0, 1] maps coherently onto seven independent coefficients in
`AmountMapper::mapAmount`. The pipeline (per stereo block):

```
in
 → DC blocker (HPF ~10 Hz)            base rate
 → WowFlutter (Lagrange3 delay + LFOs) base rate
 → TiltEQ (low + high 1-pole shelves)  base rate
 → upsample 4×
   → Saturator (tanh + asymmetry)      4× rate (only stage that aliases)
 → downsample
 → 1-pole LPF (post-saturation taming) base rate
 → LoudnessComp (makeup gain)          base rate
out
```

Drive and makeup gain ramp linearly across each block to avoid zipper noise on
coefficient changes from `AmountMapper`. The `Amount` parameter itself is
smoothed via `juce::SmoothedValue` with a 30 ms ramp before reaching the mapper.

## Customising

- **Sound character** — tweak `AmountMapper::mapAmount` curves, `Saturator::asymmetry`,
  shelf frequencies in `TiltEQ`, and the LFO rates in `WowFlutter`.
- **Plugin metadata** — change `PRODUCT_NAME`, `COMPANY_NAME`, `PLUGIN_CODE`, and
  `PLUGIN_MANUFACTURER_CODE` in `CMakeLists.txt`. **Plugin codes must be unique
  per plugin** to avoid host collisions.
- **Knob art** — replace the 10 PNGs in `Source/resources/`. Frame order is set
  in `OneKnobLookAndFeel::loadKnobFrames`.

## Testing tips

- Use JUCE's `AudioPluginHost` or your DAW to validate parameter automation and
  state save/restore.
- Sanity-check loudness with pink noise at –18 dBFS RMS at `Amount=0` vs
  `Amount=1`. Adjust `makeupDb` in `AmountMapper` if you measure more than ±0.5 LU.
- For THD inspection, feed a 1 kHz sine at –6 dBFS and run an FFT on the output.
  At full Amount the 2nd harmonic should sit around –25 to –35 dBc (knob of
  `asymmetry = 0.04` in `Saturator.h`).

## License

No license is provided here; add your own `LICENSE` before distributing.
