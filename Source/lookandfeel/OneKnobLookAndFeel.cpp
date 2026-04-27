/*
  ==============================================================================

    OneKnobLookAndFeel.cpp
    Created: Custom Look and Feel Implementation for OneKnob Plugin
    Author:  OneKnob Audio

  ==============================================================================
*/

#include "OneKnobLookAndFeel.h"
#include "BinaryData.h"

OneKnobLookAndFeel::OneKnobLookAndFeel()
{
    loadKnobFrames();
    
    // Set custom colors using our palette
    setColour(juce::Label::textColourId, OneKnobColors::getTitleColor());
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::thumbColourId, OneKnobColors::getColor(OneKnobColors::ColorRole::KnobHighlight));
}

void OneKnobLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                         int x, int y, int width, int height,
                                         float sliderPosProportional,
                                         float rotaryStartAngle,
                                         float rotaryEndAngle,
                                         juce::Slider& slider)
{
    // Calculate which frame to show based on slider position
    int frameIndex = juce::roundToInt(sliderPosProportional * (numFrames - 1));
    frameIndex = juce::jlimit(0, numFrames - 1, frameIndex);
    
    // Get the current frame
    const juce::Image& currentFrame = knobFrames[frameIndex];
    
    if (currentFrame.isValid())
    {
        // Draw the knob image directly without any clipping or background
        // Keep the original square PNG format for natural appearance
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(currentFrame, x, y, width, height, 
                   0, 0, currentFrame.getWidth(), currentFrame.getHeight());
    }
}

void OneKnobLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font(OneKnobFonts::getBodyFont());

        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);

        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                        juce::jmax(1, (int)(textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());

        g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour(label.findColour(juce::Label::outlineColourId));
    }

    g.drawRect(label.getLocalBounds());
}

void OneKnobLookAndFeel::loadKnobFrames()
{
    // Load all 10 knob frames from BinaryData (reordered to match rotation)
    knobFrames[0] = juce::ImageCache::getFromMemory(BinaryData::knob_07_png, BinaryData::knob_07_pngSize);
    knobFrames[1] = juce::ImageCache::getFromMemory(BinaryData::knob_08_png, BinaryData::knob_08_pngSize);
    knobFrames[2] = juce::ImageCache::getFromMemory(BinaryData::knob_09_png, BinaryData::knob_09_pngSize);
    knobFrames[3] = juce::ImageCache::getFromMemory(BinaryData::knob_00_png, BinaryData::knob_00_pngSize);
    knobFrames[4] = juce::ImageCache::getFromMemory(BinaryData::knob_01_png, BinaryData::knob_01_pngSize);
    knobFrames[5] = juce::ImageCache::getFromMemory(BinaryData::knob_02_png, BinaryData::knob_02_pngSize);
    knobFrames[6] = juce::ImageCache::getFromMemory(BinaryData::knob_03_png, BinaryData::knob_03_pngSize);
    knobFrames[7] = juce::ImageCache::getFromMemory(BinaryData::knob_04_png, BinaryData::knob_04_pngSize);
    knobFrames[8] = juce::ImageCache::getFromMemory(BinaryData::knob_05_png, BinaryData::knob_05_pngSize);
    knobFrames[9] = juce::ImageCache::getFromMemory(BinaryData::knob_06_png, BinaryData::knob_06_pngSize);
}