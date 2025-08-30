/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Forward declaration for RotarySliderLookAndFeel
class RotarySliderLookAndFeel;

//==============================================================================
/**
*/
class OneKnobAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OneKnobAudioProcessorEditor (OneKnobAudioProcessor&);
    ~OneKnobAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OneKnobAudioProcessor& audioProcessor;
    
    // Custom rotary slider
    juce::Slider knobSlider;
    
    // Custom look and feel
    std::unique_ptr<RotarySliderLookAndFeel> rotaryLookAndFeel;
    
    // Parameter attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    
    // Wooden frame drawing methods
    void drawWoodenFrame(juce::Graphics& g);
    void drawWoodGrain(juce::Graphics& g, juce::Rectangle<int> area);
    void drawFrameBorder(juce::Graphics& g, juce::Rectangle<int> area, int thickness);
    void drawInnerPanel(juce::Graphics& g, juce::Rectangle<int> area);
    void drawTitle(juce::Graphics& g, juce::Rectangle<int> area, int frameThickness);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobAudioProcessorEditor)
};
