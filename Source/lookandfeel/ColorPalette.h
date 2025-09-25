/*
  ==============================================================================

    ColorPalette.h
    Created: OneKnob Color Palette
    Author:  OneKnob Audio

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace OneKnobColors
{
    enum class ColorRole
    {
        // Wood frame colors
        DarkWood,
        LightWood,
        WoodGrain,
        WoodGrainDark,
        WoodGrainLight,
        
        // Frame border colors
        FrameHighlight,
        FrameShadow,
        FrameBorder,
        InnerFrameBorder,
        
        // Inner panel colors
        PanelBackground,
        PanelGradientTop,
        PanelGradientBottom,
        PanelBorder,
        PanelHighlight,
        
        // Text colors
        TitleText,
        TitleShadow,
        TitleHighlight,
        
        // Knob colors (if needed for overlays)
        KnobOverlay,
        KnobHighlight
    };
    
    // Get color by role
    juce::Colour getColor(ColorRole role);
    
    // Convenience functions for commonly used colors
    juce::Colour getDarkWood();
    juce::Colour getLightWood();
    juce::Colour getFrameHighlight();
    juce::Colour getFrameShadow();
    juce::Colour getTitleColor();
    juce::Colour getPanelBackground();
}