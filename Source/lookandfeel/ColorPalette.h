#pragma once

#include <JuceHeader.h>

namespace OneKnobColors
{
    enum class ColorRole
    {
        BackgroundTop,
        BackgroundBottom,
        VignetteEdge,

        AmberPrimary,        // glow core
        AmberSecondary,      // glow soft edge

        TextPrimary,
        TextSecondary,
        TextDim,

        // Legacy aliases — kept so older call sites keep compiling.
        // Map to the closest semantic role above.
        KnobHighlight        // ≡ AmberPrimary
    };

    juce::Colour getColor (ColorRole role);

    // Convenience accessors used across the UI.
    juce::Colour getBackgroundTop();
    juce::Colour getBackgroundBottom();
    juce::Colour getAmber();
    juce::Colour getAmberSoft();
    juce::Colour getTextPrimary();
    juce::Colour getTextSecondary();

    /** Title colour — kept for backward compatibility with existing call sites. */
    juce::Colour getTitleColor();
}
