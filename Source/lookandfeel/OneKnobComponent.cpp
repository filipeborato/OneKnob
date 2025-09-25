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
    // Configure the slider
    knobSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knobSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knobSlider.setRange(0.0, 1.0, 0.01);
    knobSlider.setValue(0.5, juce::dontSendNotification);
    
    // Set default rotation parameters (210° to 300° = 90° range)
    setRotaryParameters(juce::degreesToRadians(210.0f), juce::degreesToRadians(300.0f), true);
    
    // Create parameter attachment
    sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, parameterID, knobSlider);
    
    // Configure the label
    knobLabel.setText(labelText, juce::dontSendNotification);
    knobLabel.setJustificationType(juce::Justification::centred);
    knobLabel.setColour(juce::Label::textColourId, OneKnobColors::getTitleColor());
    knobLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    knobLabel.setFont(OneKnobFonts::getBodyFont());
    
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