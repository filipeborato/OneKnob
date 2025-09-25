/*
  ==============================================================================

    Fonts.h
    Created: OneKnob Font Management
    Author:  OneKnob Audio

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace OneKnobFonts
{
    // Font sizes
    static constexpr float HEADING_SIZE = 21.0f;
    static constexpr float BODY_SIZE = 14.0f;
    static constexpr float SMALL_SIZE = 12.0f;
    
    // Get fonts for different purposes
    juce::Font getHeadingFont();
    juce::Font getBodyFont();
    juce::Font getSmallFont();
    
    // Get font with custom size
    juce::Font getCustomFont(float size, bool bold = false);
    
    // Title font (used for plugin title)
    juce::Font getTitleFont();
}