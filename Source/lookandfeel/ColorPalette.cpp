#include "ColorPalette.h"

namespace OneKnobColors
{
    juce::Colour getColor (ColorRole role)
    {
        switch (role)
        {
            case ColorRole::BackgroundTop:    return juce::Colour (0xff222428);
            case ColorRole::BackgroundBottom: return juce::Colour (0xff0d0e10);
            case ColorRole::VignetteEdge:     return juce::Colour (0xcc000000);

            case ColorRole::AmberPrimary:     return juce::Colour (0xffffa53a);
            case ColorRole::AmberSecondary:   return juce::Colour (0xffffc264);

            case ColorRole::TextPrimary:      return juce::Colour (0xffe8e8e8);
            case ColorRole::TextSecondary:    return juce::Colour (0xff9a9a9a);
            case ColorRole::TextDim:          return juce::Colour (0xff5e5e5e);

            case ColorRole::KnobHighlight:    return juce::Colour (0xffffa53a);
        }
        return juce::Colours::black;
    }

    juce::Colour getBackgroundTop()    { return getColor (ColorRole::BackgroundTop); }
    juce::Colour getBackgroundBottom() { return getColor (ColorRole::BackgroundBottom); }
    juce::Colour getAmber()            { return getColor (ColorRole::AmberPrimary); }
    juce::Colour getAmberSoft()        { return getColor (ColorRole::AmberSecondary); }
    juce::Colour getTextPrimary()      { return getColor (ColorRole::TextPrimary); }
    juce::Colour getTextSecondary()    { return getColor (ColorRole::TextSecondary); }
    juce::Colour getTitleColor()       { return getColor (ColorRole::TextPrimary); }
}
