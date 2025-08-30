/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// RotarySliderLookAndFeel Implementation
//==============================================================================

/**
    Custom Look and Feel class for rotary sliders that uses PNG animation frames
    to create a realistic knob rotation effect.
*/
class RotarySliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RotarySliderLookAndFeel()
    {
        loadKnobFrames();
    }
    
    ~RotarySliderLookAndFeel() override = default;

    void drawRotarySlider (juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override
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

private:
    static const int numFrames = 10;
    juce::Image knobFrames[numFrames];
    
    void loadKnobFrames()
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotarySliderLookAndFeel)
};

//==============================================================================
// OneKnobAudioProcessorEditor Implementation
//==============================================================================

OneKnobAudioProcessorEditor::OneKnobAudioProcessorEditor (OneKnobAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Create the custom look and feel
    rotaryLookAndFeel = std::make_unique<RotarySliderLookAndFeel>();
    
    // Set up the knob slider with custom rotation range (210° to 300°)
    knobSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knobSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    knobSlider.setRange(0.0, 1.0, 0.01);
    
    // Initialize knob at middle position (corresponds to frame 4-5, which should be visually middle)
    knobSlider.setValue(0.5, juce::dontSendNotification);
    
    // Convert degrees to radians and set rotary parameters
    // 210° to 300° gives us 90° of rotation range
    float startAngle = juce::degreesToRadians(210.0f);
    float endAngle = juce::degreesToRadians(300.0f);
    knobSlider.setRotaryParameters(startAngle, endAngle, true);
    
    knobSlider.setLookAndFeel(rotaryLookAndFeel.get());
    
    // Create parameter attachment
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "gain", knobSlider);
    
    // Add the slider to the editor
    addAndMakeVisible(knobSlider);
    
    // Increased size by 30% (from 400x300 to 520x390)
    setSize (520, 390);
}

OneKnobAudioProcessorEditor::~OneKnobAudioProcessorEditor()
{
    // Reset look and feel to avoid dangling pointer
    knobSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void OneKnobAudioProcessorEditor::paint (juce::Graphics& g)
{
    drawWoodenFrame(g);
}

void OneKnobAudioProcessorEditor::drawWoodenFrame(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Define frame thickness - increased by 30% (from 25 to 33)
    const int frameThickness = 33;
    
    // Main background - dark wood color
    juce::Colour darkWood(0xff3d2914);  // Dark brown wood
    juce::Colour lightWood(0xff8b4513); // Saddle brown
    juce::Colour woodGrain(0xff654321); // Medium brown
    
    // Fill the entire background with dark wood
    g.setColour(darkWood);
    g.fillAll();
    
    // Create wood grain pattern using gradients
    drawWoodGrain(g, bounds);
    
    // Draw the outer frame border
    drawFrameBorder(g, bounds, frameThickness);
    
    // Inner panel area (where the knob sits)
    auto innerArea = bounds.reduced(frameThickness);
    drawInnerPanel(g, innerArea);
    
    // Draw title on the wooden frame
    drawTitle(g, bounds, frameThickness);
}

void OneKnobAudioProcessorEditor::drawWoodGrain(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Create vertical wood grain lines - spacing increased by 30%
    juce::Colour grainColor1(0xff5d4037); // Darker grain
    juce::Colour grainColor2(0xff8d6e63); // Lighter grain
    
    for (int i = 0; i < area.getWidth(); i += 10) // Increased from 8 to 10
    {
        // Vertical grain lines with slight variation
        float alpha = 0.3f + (std::sin(i * 0.1f) * 0.2f);
        g.setColour(grainColor1.withAlpha(alpha));
        g.drawVerticalLine(area.getX() + i, area.getY(), area.getBottom());
        
        if (i % 21 == 0) // Increased from 16 to 21
        {
            g.setColour(grainColor2.withAlpha(alpha * 0.5f));
            g.drawVerticalLine(area.getX() + i + 1, area.getY(), area.getBottom());
        }
    }
    
    // Add some horizontal grain variation - spacing increased by 30%
    for (int j = 0; j < area.getHeight(); j += 33) // Increased from 25 to 33
    {
        float alpha = 0.1f + (std::sin(j * 0.05f) * 0.1f);
        g.setColour(grainColor1.withAlpha(alpha));
        g.drawHorizontalLine(area.getY() + j, area.getX(), area.getRight());
    }
}

void OneKnobAudioProcessorEditor::drawFrameBorder(juce::Graphics& g, juce::Rectangle<int> area, int thickness)
{
    // Create beveled frame border effect
    
    // Outer highlights (top and left) - thickness increased by 30%
    g.setColour(juce::Colour(0xffb8860b)); // Dark goldenrod highlight
    g.fillRect(0, 0, area.getWidth(), 3); // Increased from 2 to 3
    g.fillRect(0, 0, 3, area.getHeight()); // Increased from 2 to 3
    
    // Inner shadows (bottom and right) - thickness increased by 30%
    g.setColour(juce::Colour(0xff2f1b14)); // Very dark brown shadow
    g.fillRect(0, area.getHeight() - 3, area.getWidth(), 3); // Increased from 2 to 3
    g.fillRect(area.getWidth() - 3, 0, 3, area.getHeight()); // Increased from 2 to 3
    
    // Middle frame area with gradient
    auto frameArea = area.reduced(3); // Increased from 2 to 3
    juce::ColourGradient frameGradient(
        juce::Colour(0xff8b4513), frameArea.getX(), frameArea.getY(),
        juce::Colour(0xff654321), frameArea.getRight(), frameArea.getBottom(),
        false);
    
    g.setGradientFill(frameGradient);
    
    // Draw frame border with thickness
    g.fillRect(frameArea.getX(), frameArea.getY(), frameArea.getWidth(), thickness); // Top
    g.fillRect(frameArea.getX(), frameArea.getBottom() - thickness, frameArea.getWidth(), thickness); // Bottom
    g.fillRect(frameArea.getX(), frameArea.getY(), thickness, frameArea.getHeight()); // Left
    g.fillRect(frameArea.getRight() - thickness, frameArea.getY(), thickness, frameArea.getHeight()); // Right
    
    // Inner frame highlights
    auto innerFrame = frameArea.reduced(thickness - 4); // Increased from 3 to 4
    g.setColour(juce::Colour(0xff4a2c17)); // Dark inner border
    g.drawRect(innerFrame, 1);
}

void OneKnobAudioProcessorEditor::drawInnerPanel(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Create a slightly recessed inner panel
    juce::ColourGradient panelGradient(
        juce::Colour(0xff2d1810), area.getCentreX(), area.getY(),
        juce::Colour(0xff1a0f08), area.getCentreX(), area.getBottom(),
        false);
    
    g.setGradientFill(panelGradient);
    g.fillRect(area);
    
    // Add inner panel border
    g.setColour(juce::Colour(0xff1a0f08));
    g.drawRect(area, 1);
    
    // Subtle inner highlight
    g.setColour(juce::Colour(0xff3d2914).withAlpha(0.5f));
    g.drawRect(area.reduced(1), 1);
}

void OneKnobAudioProcessorEditor::drawTitle(juce::Graphics& g, juce::Rectangle<int> area, int frameThickness)
{
    // Draw title on the top frame
    auto titleArea = juce::Rectangle<int>(0, 3, area.getWidth(), frameThickness - 6); // Adjusted for new thickness
    
    // Title text with wood carving effect - font size increased by 30%
    g.setColour(juce::Colour(0xff1a0f08)); // Very dark shadow
    g.setFont(juce::FontOptions(21.0f, juce::Font::bold)); // Increased from 16.0f to 21.0f
    g.drawFittedText("OneKnob", titleArea.translated(1, 1), juce::Justification::centred, 1);
    
    // Main title text
    g.setColour(juce::Colour(0xffd4af37)); // Golden color like carved lettering
    g.drawFittedText("OneKnob", titleArea, juce::Justification::centred, 1);
    
    // Subtle highlight on title
    g.setColour(juce::Colour(0xfffff8dc).withAlpha(0.6f)); // Cornsilk highlight
    g.drawFittedText("OneKnob", titleArea.translated(0, -1), juce::Justification::centred, 1);
}

void OneKnobAudioProcessorEditor::resized()
{
    // Position the knob slider in the center of the inner panel area
    auto bounds = getLocalBounds();
    const int frameThickness = 33; // Updated to match new thickness
    auto innerArea = bounds.reduced(frameThickness);
    
    // Center the knob in the inner area - padding increased by 30%
    int knobSize = juce::jmin(innerArea.getWidth(), innerArea.getHeight()) - 52; // Increased from 40 to 52
    knobSlider.setBounds(innerArea.getCentreX() - knobSize/2, 
                        innerArea.getCentreY() - knobSize/2, 
                        knobSize, knobSize);
}
