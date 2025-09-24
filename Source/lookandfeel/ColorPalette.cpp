/*
  ==============================================================================

    ColorPalette.cpp
    Created: OneKnob Color Palette Implementation
    Author:  OneKnob Audio

  ==============================================================================
*/

#include "ColorPalette.h"

namespace OneKnobColors
{
    juce::Colour getColor(ColorRole role)
    {
        switch (role)
        {
            // Wood frame colors
            case ColorRole::DarkWood:       return juce::Colour(0xff3d2914);
            case ColorRole::LightWood:      return juce::Colour(0xff8b4513);
            case ColorRole::WoodGrain:      return juce::Colour(0xff654321);
            case ColorRole::WoodGrainDark:  return juce::Colour(0xff5d4037);
            case ColorRole::WoodGrainLight: return juce::Colour(0xff8d6e63);
            
            // Frame border colors
            case ColorRole::FrameHighlight:   return juce::Colour(0xffb8860b);
            case ColorRole::FrameShadow:      return juce::Colour(0xff2f1b14);
            case ColorRole::FrameBorder:      return juce::Colour(0xff8b4513);
            case ColorRole::InnerFrameBorder: return juce::Colour(0xff4a2c17);
            
            // Inner panel colors
            case ColorRole::PanelBackground:    return juce::Colour(0xff2d1810);
            case ColorRole::PanelGradientTop:   return juce::Colour(0xff2d1810);
            case ColorRole::PanelGradientBottom:return juce::Colour(0xff1a0f08);
            case ColorRole::PanelBorder:        return juce::Colour(0xff1a0f08);
            case ColorRole::PanelHighlight:     return juce::Colour(0xff3d2914);
            
            // Text colors
            case ColorRole::TitleText:      return juce::Colour(0xffd4af37);
            case ColorRole::TitleShadow:    return juce::Colour(0xff1a0f08);
            case ColorRole::TitleHighlight: return juce::Colour(0xfffff8dc);
            
            // Knob colors
            case ColorRole::KnobOverlay:    return juce::Colour(0x80ffffff);
            case ColorRole::KnobHighlight:  return juce::Colour(0xffd4af37);
            
            default:
                return juce::Colours::black;
        }
    }
    
    juce::Colour getDarkWood()
    {
        return getColor(ColorRole::DarkWood);
    }
    
    juce::Colour getLightWood()
    {
        return getColor(ColorRole::LightWood);
    }
    
    juce::Colour getFrameHighlight()
    {
        return getColor(ColorRole::FrameHighlight);
    }
    
    juce::Colour getFrameShadow()
    {
        return getColor(ColorRole::FrameShadow);
    }
    
    juce::Colour getTitleColor()
    {
        return getColor(ColorRole::TitleText);
    }
    
    juce::Colour getPanelBackground()
    {
        return getColor(ColorRole::PanelBackground);
    }
}