/*
  ==============================================================================

    OneKnobLookAndFeel.h
    Created: Custom Look and Feel for OneKnob Plugin
    Author:  OneKnob Audio

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"
#include "Fonts.h"

/**
    Custom Look and Feel class for OneKnob plugin that centralizes all visual styling.
    Handles rotary sliders with PNG animation frames and consistent color scheme.
*/
class OneKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OneKnobLookAndFeel();
    ~OneKnobLookAndFeel() override = default;

    // Rotary slider drawing
    void drawRotarySlider(juce::Graphics& g,
                         int x, int y, int width, int height,
                         float sliderPosProportional,
                         float rotaryStartAngle,
                         float rotaryEndAngle,
                         juce::Slider& slider) override;

    // Label drawing (for component labels)
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

private:
    static const int numFrames = 10;
    juce::Image knobFrames[numFrames];
    
    void loadKnobFrames();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OneKnobLookAndFeel)
};