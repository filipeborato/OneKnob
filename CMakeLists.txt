# =============================================================================
# OneKnob — JUCE 8 audio plugin
# =============================================================================
cmake_minimum_required(VERSION 3.22)

# ---- Project ----------------------------------------------------------------
project(OneKnob
    VERSION     0.1.0
    DESCRIPTION "OneKnob — minimal one-knob JUCE audio plugin"
    LANGUAGES   C CXX
)

# Out-of-source builds only.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed. Use: cmake -S . -B build")
endif()

# C++20 — matches the cppLanguageStandard set in OneKnob.jucer.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sane default for single-config generators (Ninja, Makefiles).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

# Useful for clangd / IDE tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Group targets in IDEs (VS, Xcode).
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---- User-facing options ----------------------------------------------------
option(ONEKNOB_BUILD_VST3        "Build VST3 plugin"                          ON)
option(ONEKNOB_BUILD_STANDALONE  "Build Standalone application"               ON)
option(ONEKNOB_BUILD_AU          "Build Audio Unit (Apple only)"              ON)
option(ONEKNOB_ENABLE_LTO        "Enable Link Time Optimisation in Release"   ON)
option(ONEKNOB_WARNINGS_AS_ERRORS "Treat warnings as errors"                  OFF)
option(ONEKNOB_COPY_PLUGIN_AFTER_BUILD
    "Install plugin to system location after build (dev convenience)"         OFF)

set(ONEKNOB_COMPANY_NAME    "Borato Company"
    CACHE STRING "Company / vendor name shown by the host")
set(ONEKNOB_COMPANY_WEBSITE "https://example.com"
    CACHE STRING "Company website (used as bundle metadata)")
set(ONEKNOB_BUNDLE_ID       "com.oneknobaudio.OneKnob"
    CACHE STRING "Reverse-DNS bundle identifier (must be unique per plugin)")

# JUCE pin. Override at configure time with -DJUCE_TAG=<tag-or-branch>.
# JUCE 8 is required: this project uses the juce_animation and
# juce_audio_processors_headless modules (the latter requires ≥ 8.0.5).
set(JUCE_TAG "8.0.12"
    CACHE STRING "JUCE git tag, branch, or commit to fetch")

# ---- JUCE -------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        ${JUCE_TAG}
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(JUCE)

# ---- Plugin formats ---------------------------------------------------------
set(_oneknob_formats "")
if(ONEKNOB_BUILD_VST3)
    list(APPEND _oneknob_formats VST3)
endif()
if(ONEKNOB_BUILD_STANDALONE)
    list(APPEND _oneknob_formats Standalone)
endif()
if(ONEKNOB_BUILD_AU AND APPLE)
    list(APPEND _oneknob_formats AU)
endif()

if(NOT _oneknob_formats)
    message(FATAL_ERROR
        "No plugin format selected. Enable at least one of "
        "ONEKNOB_BUILD_VST3 / ONEKNOB_BUILD_STANDALONE / ONEKNOB_BUILD_AU.")
endif()

# ---- Plugin target ----------------------------------------------------------
juce_add_plugin(OneKnob
    PRODUCT_NAME                "OneKnob"
    COMPANY_NAME                "${ONEKNOB_COMPANY_NAME}"
    COMPANY_WEBSITE             "${ONEKNOB_COMPANY_WEBSITE}"
    BUNDLE_ID                   "${ONEKNOB_BUNDLE_ID}"
    PLUGIN_MANUFACTURER_CODE    OnKb     # 4 chars, first uppercase
    PLUGIN_CODE                 Onkb     # 4 chars, unique per plugin
    FORMATS                     ${_oneknob_formats}
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     ${ONEKNOB_COPY_PLUGIN_AFTER_BUILD}
    VST3_CATEGORIES             "Fx" "Dynamics"
    AU_MAIN_TYPE                "kAudioUnitType_Effect"
)

# Generate JuceHeader.h so the existing `#include <JuceHeader.h>` keeps working.
juce_generate_juce_header(OneKnob)

# ---- Sources ----------------------------------------------------------------
target_sources(OneKnob PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h

    Source/dsp/AmountMapper.h
    Source/dsp/OnePoleFilters.h
    Source/dsp/Saturator.h
    Source/dsp/TiltEQ.h
    Source/dsp/WowFlutter.h
    Source/dsp/LoudnessComp.h
    Source/dsp/TapeProcessor.h
    Source/dsp/TapeProcessor.cpp

    Source/lookandfeel/ColorPalette.cpp
    Source/lookandfeel/ColorPalette.h
    Source/lookandfeel/Fonts.cpp
    Source/lookandfeel/Fonts.h
    Source/lookandfeel/OneKnobComponent.cpp
    Source/lookandfeel/OneKnobComponent.h
    Source/lookandfeel/OneKnobLookAndFeel.cpp
    Source/lookandfeel/OneKnobLookAndFeel.h
)

target_include_directories(OneKnob PRIVATE Source)

# Pretty IDE folders.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source"
    FILES
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/dsp/AmountMapper.h
        Source/dsp/OnePoleFilters.h
        Source/dsp/Saturator.h
        Source/dsp/TiltEQ.h
        Source/dsp/WowFlutter.h
        Source/dsp/LoudnessComp.h
        Source/dsp/TapeProcessor.h
        Source/dsp/TapeProcessor.cpp
        Source/lookandfeel/ColorPalette.cpp
        Source/lookandfeel/ColorPalette.h
        Source/lookandfeel/Fonts.cpp
        Source/lookandfeel/Fonts.h
        Source/lookandfeel/OneKnobComponent.cpp
        Source/lookandfeel/OneKnobComponent.h
        Source/lookandfeel/OneKnobLookAndFeel.cpp
        Source/lookandfeel/OneKnobLookAndFeel.h
)

# ---- Embedded resources (animated knob frames) ------------------------------
juce_add_binary_data(OneKnobBinaryData
    HEADER_NAME BinaryData.h
    NAMESPACE   BinaryData
    SOURCES
        Source/resources/knob_00.png
        Source/resources/knob_01.png
        Source/resources/knob_02.png
        Source/resources/knob_03.png
        Source/resources/knob_04.png
        Source/resources/knob_05.png
        Source/resources/knob_06.png
        Source/resources/knob_07.png
        Source/resources/knob_08.png
        Source/resources/knob_09.png
)

# Position-independent code is required when consumed by the shared plugin
# binaries (VST3, AU bundles).
set_target_properties(OneKnobBinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    FOLDER "OneKnob"
)

# ---- Compile definitions ----------------------------------------------------
target_compile_definitions(OneKnob PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_REPORT_APP_USAGE=0
    JUCE_MODAL_LOOPS_PERMITTED=0
)

# ---- Link -------------------------------------------------------------------
# juce_audio_plugin_client is added implicitly by juce_add_plugin.
target_link_libraries(OneKnob
    PRIVATE
        OneKnobBinaryData
        juce::juce_animation
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_processors_headless
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

if(ONEKNOB_ENABLE_LTO)
    target_link_libraries(OneKnob PUBLIC juce::juce_recommended_lto_flags)
endif()

# ---- Extra warnings / hardening --------------------------------------------
if(MSVC)
    target_compile_options(OneKnob PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
        /Zc:preprocessor
        /MP                 # parallel compilation
    )
    if(ONEKNOB_WARNINGS_AS_ERRORS)
        target_compile_options(OneKnob PRIVATE /WX)
    endif()
else()
    target_compile_options(OneKnob PRIVATE
        -Wall -Wextra -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
    )
    if(ONEKNOB_WARNINGS_AS_ERRORS)
        target_compile_options(OneKnob PRIVATE -Werror)
    endif()
endif()
