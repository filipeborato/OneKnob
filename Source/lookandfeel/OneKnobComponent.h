/*
  ==============================================================================

    OneKnobComponent.h
    Created: Wrapper component for OneKnob Slider with Label
    Author:  OneKnob Audio

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OneKnobLookAndFeel.h"

/**
    A complete knob component that includes a slider and optional label.
    Pre-configured with OneKnob look and feel and parameter attachment.
*/
class OneKnobComponent : public juce::Component
{
public:
    OneKnobComponent(const juce::String& parameterID, 
                    juce::AudioProcessorValueTreeState& valueTreeState,
                    const juce::String& labelText = "");
    
    ~OneKnobComponent() override;

    // Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Access to the slider (for additional configuration if needed)
    juce::Slider& getSlider() { return knobSlider; }
    juce::Label& getLabel() { return knobLabel; }
    
    // Set label visibility
    void setLabelVisible(bool shouldBeVisible);
    
    // Set custom rotation parameters
    void setRotaryParameters(float startAngleRadians, float endAngleRadians, bool stopAtEnd = true);

private:
    juce::Slider knobSlider;
    juce::Label knobLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
    
    bool labelVisible = true;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OneKnobComponent)
};