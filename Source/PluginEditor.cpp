#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "lookandfeel/ColorPalette.h"
#include "lookandfeel/Fonts.h"

namespace
{
    constexpr int kEditorWidth   = 460;
    constexpr int kEditorHeight  = 360;
    constexpr int kKnobSize      = 200;
    constexpr int kTitleHeight   = 28;
    constexpr int kValueHeight   = 22;

    constexpr float kHaloMaxAlpha = 0.55f;
}

OneKnobAudioProcessorEditor::OneKnobAudioProcessorEditor (OneKnobAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    oneKnobLookAndFeel = std::make_unique<OneKnobLookAndFeel>();
    setLookAndFeel (oneKnobLookAndFeel.get());

    knobComponent = std::make_unique<OneKnobComponent> (
        OneKnobAudioProcessor::kAmountParamID,
        audioProcessor.getValueTreeState(),
        "TAPE");
    addAndMakeVisible (*knobComponent);

    valueLabel.setText ("0 %", juce::dontSendNotification);
    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setColour (juce::Label::textColourId, OneKnobColors::getTextSecondary());
    valueLabel.setFont (OneKnobFonts::getCustomFont (12.0f, false));
    addAndMakeVisible (valueLabel);

    setSize (kEditorWidth, kEditorHeight);

    currentAmount = static_cast<float> (knobComponent->getSlider().getValue());
    startTimerHz (30);
}

OneKnobAudioProcessorEditor::~OneKnobAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void OneKnobAudioProcessorEditor::timerCallback()
{
    const float v = static_cast<float> (knobComponent->getSlider().getValue());
    if (std::abs (v - currentAmount) > 1.0e-4f)
    {
        currentAmount = v;
        valueLabel.setText (juce::String (juce::roundToInt (v * 100.0f)) + " %",
                            juce::dontSendNotification);
        repaint();
    }
}

void OneKnobAudioProcessorEditor::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // 1) Background — vertical gradient charcoal.
    juce::ColourGradient bg (
        OneKnobColors::getBackgroundTop(),    bounds.getCentreX(), bounds.getY(),
        OneKnobColors::getBackgroundBottom(), bounds.getCentreX(), bounds.getBottom(),
        false);
    g.setGradientFill (bg);
    g.fillAll();

    // 2) Vignette — radial darkening at the corners.
    {
        const float cx = bounds.getCentreX();
        const float cy = bounds.getCentreY();
        const float r  = juce::jmax (bounds.getWidth(), bounds.getHeight()) * 0.75f;

        juce::ColourGradient vignette (
            juce::Colours::transparentBlack, cx, cy,
            OneKnobColors::getColor (OneKnobColors::ColorRole::VignetteEdge), cx + r, cy,
            true);
        g.setGradientFill (vignette);
        g.fillAll();
    }

    // 3) Amber halo behind the knob — intensity tracks Amount.
    if (knobComponent != nullptr && currentAmount > 1.0e-3f)
    {
        const auto knobBounds = knobComponent->getBounds().toFloat();
        const auto centre     = knobBounds.getCentre();
        const float maxRadius = knobBounds.getWidth() * 0.85f;
        const float alpha     = kHaloMaxAlpha * currentAmount;

        juce::ColourGradient halo (
            OneKnobColors::getAmber().withAlpha (alpha),     centre.x, centre.y,
            OneKnobColors::getAmber().withAlpha (0.0f),      centre.x + maxRadius, centre.y,
            true);
        g.setGradientFill (halo);
        g.fillEllipse (centre.x - maxRadius, centre.y - maxRadius,
                       maxRadius * 2.0f, maxRadius * 2.0f);
    }

    // 4) Title.
    g.setColour (OneKnobColors::getTextPrimary().withAlpha (0.85f));
    g.setFont (OneKnobFonts::getCustomFont (12.0f, true));
    g.drawText ("ONEKNOB",
                juce::Rectangle<int> (0, 16, getWidth(), kTitleHeight),
                juce::Justification::centred);

    // 5) Thin amber accent line under the title — subtle, always present.
    g.setColour (OneKnobColors::getAmber().withAlpha (0.35f));
    const float lineY = 16.0f + kTitleHeight + 2.0f;
    g.fillRect (juce::Rectangle<float> (bounds.getCentreX() - 18.0f, lineY, 36.0f, 1.0f));
}

void OneKnobAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    bounds.removeFromTop (16 + kTitleHeight + 8);          // title gutter
    auto bottom = bounds.removeFromBottom (kValueHeight + 16);
    valueLabel.setBounds (bottom.withTrimmedBottom (10));

    // Centre the knob horizontally and vertically in what remains.
    const int knobX = (getWidth() - kKnobSize) / 2;
    const int knobY = bounds.getY() + (bounds.getHeight() - kKnobSize) / 2;
    knobComponent->setBounds (knobX, knobY, kKnobSize, kKnobSize);
}
