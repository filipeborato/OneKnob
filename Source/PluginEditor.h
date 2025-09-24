/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "lookandfeel/OneKnobLookAndFeel.h"
#include "lookandfeel/OneKnobComponent.h"
#include "lookandfeel/ColorPalette.h"

//==============================================================================
/**
    Main plugin editor that handles layout and wooden frame background.
    All look and feel logic is now centralized in OneKnobLookAndFeel.
*/
class OneKnobAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    OneKnobAudioProcessorEditor(OneKnobAudioProcessor&);
    ~OneKnobAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to the processor
    OneKnobAudioProcessor& audioProcessor;
    
    // Custom look and feel (centralized styling)
    std::unique_ptr<OneKnobLookAndFeel> oneKnobLookAndFeel;
    
    // Main gain knob component
    std::unique_ptr<OneKnobComponent> gainKnobComponent;
    
    // Wooden frame drawing methods (background only)
    void drawWoodenFrame(juce::Graphics& g);
    void drawWoodGrain(juce::Graphics& g, juce::Rectangle<int> area);
    void drawFrameBorder(juce::Graphics& g, juce::Rectangle<int> area, int thickness);
    void drawInnerPanel(juce::Graphics& g, juce::Rectangle<int> area);
    void drawTitle(juce::Graphics& g, juce::Rectangle<int> area, int frameThickness);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OneKnobAudioProcessorEditor)
};
