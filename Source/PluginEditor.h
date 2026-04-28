#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "lookandfeel/OneKnobLookAndFeel.h"
#include "lookandfeel/OneKnobComponent.h"

/** Modern minimal one-knob editor: charcoal panel with an amber halo
    behind the knob whose intensity tracks the Amount parameter. */
class OneKnobAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    private juce::Timer
{
public:
    explicit OneKnobAudioProcessorEditor (OneKnobAudioProcessor&);
    ~OneKnobAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    OneKnobAudioProcessor& audioProcessor;

    std::unique_ptr<OneKnobLookAndFeel> oneKnobLookAndFeel;
    std::unique_ptr<OneKnobComponent>   knobComponent;

    juce::Label valueLabel;        // shows current % below the knob

    float currentAmount = 0.0f;    // mirrored from the slider for paint()

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobAudioProcessorEditor)
};
