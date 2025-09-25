/*
  ==============================================================================

    Fonts.cpp
    Created: OneKnob Font Management Implementation
    Author:  OneKnob Audio

  ==============================================================================
*/

#include "Fonts.h"

namespace OneKnobFonts
{
    juce::Font getHeadingFont()
    {
        return juce::FontOptions(HEADING_SIZE, juce::Font::bold);
    }
    
    juce::Font getBodyFont()
    {
        return juce::FontOptions(BODY_SIZE, juce::Font::plain);
    }
    
    juce::Font getSmallFont()
    {
        return juce::FontOptions(SMALL_SIZE, juce::Font::plain);
    }
    
    juce::Font getCustomFont(float size, bool bold)
    {
        return juce::FontOptions(size, bold ? juce::Font::bold : juce::Font::plain);
    }
    
    juce::Font getTitleFont()
    {
        return juce::FontOptions(HEADING_SIZE, juce::Font::bold);
    }
}