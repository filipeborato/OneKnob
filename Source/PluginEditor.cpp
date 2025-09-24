/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "lookandfeel/ColorPalette.h"
#include "lookandfeel/Fonts.h"

//==============================================================================
// OneKnobAudioProcessorEditor Implementation
//==============================================================================

OneKnobAudioProcessorEditor::OneKnobAudioProcessorEditor(OneKnobAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Create the centralized look and feel
    oneKnobLookAndFeel = std::make_unique<OneKnobLookAndFeel>();
    
    // Set the look and feel globally for this editor
    setLookAndFeel(oneKnobLookAndFeel.get());
    
    // Create the main gain knob component with "gain" parameter
    gainKnobComponent = std::make_unique<OneKnobComponent>(
        "gain", 
        audioProcessor.getValueTreeState(),
        "GAIN"
    );
    
    // Add the knob component to the editor
    addAndMakeVisible(*gainKnobComponent);
    
    // Set window size (increased by 30% from original 400x300)
    setSize(520, 390);
}

OneKnobAudioProcessorEditor::~OneKnobAudioProcessorEditor()
{
    // Reset look and feel to avoid dangling pointer
    setLookAndFeel(nullptr);
}

//==============================================================================
void OneKnobAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Only handle the wooden frame background - knob drawing is handled by OneKnobLookAndFeel
    drawWoodenFrame(g);
}

void OneKnobAudioProcessorEditor::drawWoodenFrame(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Define frame thickness - increased by 30% (from 25 to 33)
    const int frameThickness = 33;
    
    // Fill the entire background with dark wood
    g.setColour(OneKnobColors::getDarkWood());
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
    // Create vertical wood grain lines using color palette
    auto grainColor1 = OneKnobColors::getColor(OneKnobColors::ColorRole::WoodGrainDark);
    auto grainColor2 = OneKnobColors::getColor(OneKnobColors::ColorRole::WoodGrainLight);
    
    for (int i = 0; i < area.getWidth(); i += 10) // Increased spacing
    {
        // Vertical grain lines with slight variation
        float alpha = 0.3f + (std::sin(static_cast<float>(i) * 0.1f) * 0.2f);
        g.setColour(grainColor1.withAlpha(alpha));
        g.drawVerticalLine(area.getX() + i, static_cast<float>(area.getY()), static_cast<float>(area.getBottom()));
        
        if (i % 21 == 0) // Increased spacing
        {
            g.setColour(grainColor2.withAlpha(alpha * 0.5f));
            g.drawVerticalLine(area.getX() + i + 1, static_cast<float>(area.getY()), static_cast<float>(area.getBottom()));
        }
    }
    
    // Add some horizontal grain variation
    for (int j = 0; j < area.getHeight(); j += 33) // Increased spacing
    {
        float alpha = 0.1f + (std::sin(static_cast<float>(j) * 0.05f) * 0.1f);
        g.setColour(grainColor1.withAlpha(alpha));
        g.drawHorizontalLine(area.getY() + j, static_cast<float>(area.getX()), static_cast<float>(area.getRight()));
    }
}

void OneKnobAudioProcessorEditor::drawFrameBorder(juce::Graphics& g, juce::Rectangle<int> area, int thickness)
{
    // Create beveled frame border effect using color palette
    
    // Outer highlights (top and left)
    g.setColour(OneKnobColors::getFrameHighlight());
    g.fillRect(0, 0, area.getWidth(), 3);
    g.fillRect(0, 0, 3, area.getHeight());
    
    // Inner shadows (bottom and right)
    g.setColour(OneKnobColors::getFrameShadow());
    g.fillRect(0, area.getHeight() - 3, area.getWidth(), 3);
    g.fillRect(area.getWidth() - 3, 0, 3, area.getHeight());
    
    // Middle frame area with gradient
    auto frameArea = area.reduced(3);
    juce::ColourGradient frameGradient(
        OneKnobColors::getLightWood(), static_cast<float>(frameArea.getX()), static_cast<float>(frameArea.getY()),
        OneKnobColors::getColor(OneKnobColors::ColorRole::WoodGrain), static_cast<float>(frameArea.getRight()), static_cast<float>(frameArea.getBottom()),
        false);
    
    g.setGradientFill(frameGradient);
    
    // Draw frame border with thickness
    g.fillRect(frameArea.getX(), frameArea.getY(), frameArea.getWidth(), thickness); // Top
    g.fillRect(frameArea.getX(), frameArea.getBottom() - thickness, frameArea.getWidth(), thickness); // Bottom
    g.fillRect(frameArea.getX(), frameArea.getY(), thickness, frameArea.getHeight()); // Left
    g.fillRect(frameArea.getRight() - thickness, frameArea.getY(), thickness, frameArea.getHeight()); // Right
    
    // Inner frame highlights
    auto innerFrame = frameArea.reduced(thickness - 4);
    g.setColour(OneKnobColors::getColor(OneKnobColors::ColorRole::InnerFrameBorder));
    g.drawRect(innerFrame, 1);
}

void OneKnobAudioProcessorEditor::drawInnerPanel(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Create a slightly recessed inner panel using color palette
    juce::ColourGradient panelGradient(
        OneKnobColors::getColor(OneKnobColors::ColorRole::PanelGradientTop), static_cast<float>(area.getCentreX()), static_cast<float>(area.getY()),
        OneKnobColors::getColor(OneKnobColors::ColorRole::PanelGradientBottom), static_cast<float>(area.getCentreX()), static_cast<float>(area.getBottom()),
        false);
    
    g.setGradientFill(panelGradient);
    g.fillRect(area);
    
    // Add inner panel border
    g.setColour(OneKnobColors::getColor(OneKnobColors::ColorRole::PanelBorder));
    g.drawRect(area, 1);
    
    // Subtle inner highlight
    g.setColour(OneKnobColors::getColor(OneKnobColors::ColorRole::PanelHighlight).withAlpha(0.5f));
    g.drawRect(area.reduced(1), 1);
}

void OneKnobAudioProcessorEditor::drawTitle(juce::Graphics& g, juce::Rectangle<int> area, int frameThickness)
{
    // Draw title on the top frame using color palette and font system
    auto titleArea = juce::Rectangle<int>(0, 3, area.getWidth(), frameThickness - 6);
    
    // Title text with wood carving effect
    g.setColour(OneKnobColors::getColor(OneKnobColors::ColorRole::TitleShadow));
    g.setFont(OneKnobFonts::getTitleFont());
    g.drawFittedText("OneKnob", titleArea.translated(1, 1), juce::Justification::centred, 1);
    
    // Main title text
    g.setColour(OneKnobColors::getTitleColor());
    g.drawFittedText("OneKnob", titleArea, juce::Justification::centred, 1);
    
    // Subtle highlight on title
    g.setColour(OneKnobColors::getColor(OneKnobColors::ColorRole::TitleHighlight).withAlpha(0.6f));
    g.drawFittedText("OneKnob", titleArea.translated(0, -1), juce::Justification::centred, 1);
}

void OneKnobAudioProcessorEditor::resized()
{
    // Position the gain knob component in the center of the inner panel area
    auto bounds = getLocalBounds();
    const int frameThickness = 33;
    auto innerArea = bounds.reduced(frameThickness);
    
    // Center the knob in the inner area with padding
    int knobSize = juce::jmin(innerArea.getWidth(), innerArea.getHeight()) - 52;
    int knobX = innerArea.getCentreX() - knobSize / 2;
    int knobY = innerArea.getCentreY() - knobSize / 2;
    
    gainKnobComponent->setBounds(knobX, knobY, knobSize, knobSize);
}
