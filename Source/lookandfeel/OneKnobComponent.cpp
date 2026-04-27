/*
  ==============================================================================

    OneKnobComponent.cpp
    Created: Wrapper component implementation for OneKnob Slider with Label
    Author:  OneKnob Audio

  ==============================================================================
*/

#include "OneKnobComponent.h"
#include "ColorPalette.h"
#include "Fonts.h"

OneKnobComponent::OneKnobComponent(const juce::String& parameterID, 
                                 juce::AudioProcessorValueTreeState& valueTreeState,
                                 const juce::String& labelText)
{
    // Configure the slider — range/step matches the AVTS parameter (0..1, fine resolution).
    knobSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knobSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knobSlider.setRange(0.0, 1.0, 0.001);
    knobSlider.setDoubleClickReturnValue(true, 0.25);

    // 270-degree arc, 7-o'clock to 5-o'clock — natural for a single rotary.
    setRotaryParameters(juce::degreesToRadians(225.0f), juce::degreesToRadians(495.0f), true);

    // Create parameter attachment (this overrides the slider's initial value with the AVTS one).
    sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, parameterID, knobSlider);

    // Configure the label — small, dim, letter-tracked uppercase.
    knobLabel.setText(labelText, juce::dontSendNotification);
    knobLabel.setJustificationType(juce::Justification::centred);
    knobLabel.setColour(juce::Label::textColourId, OneKnobColors::getTextSecondary());
    knobLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    knobLabel.setFont(OneKnobFonts::getCustomFont(13.0f, true));
    
    // Add components
    addAndMakeVisible(knobSlider);
    
    if (!labelText.isEmpty())
    {
        addAndMakeVisible(knobLabel);
        labelVisible = true;
    }
    else
    {
        labelVisible = false;
    }
}

OneKnobComponent::~OneKnobComponent()
{
    // Attachment will be automatically cleaned up by unique_ptr
}

void OneKnobComponent::paint(juce::Graphics& g)
{
    // The knob drawing is handled by the look and feel
    // This component is just a container
}

void OneKnobComponent::resized()
{
    auto bounds = getLocalBounds();
    
    if (labelVisible && knobLabel.isVisible())
    {
        // Reserve space for label at the bottom
        const int labelHeight = 20;
        knobLabel.setBounds(bounds.removeFromBottom(labelHeight));
        bounds.removeFromBottom(5); // Small gap between knob and label
    }
    
    // The remaining space is for the knob
    knobSlider.setBounds(bounds);
}

void OneKnobComponent::setLabelVisible(bool shouldBeVisible)
{
    labelVisible = shouldBeVisible;
    knobLabel.setVisible(shouldBeVisible);
    resized();
}

void OneKnobComponent::setRotaryParameters(float startAngleRadians, float endAngleRadians, bool stopAtEnd)
{
    knobSlider.setRotaryParameters(startAngleRadians, endAngleRadians, stopAtEnd);
}