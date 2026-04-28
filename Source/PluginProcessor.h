#pragma once

#include <JuceHeader.h>
#include <atomic>

#include "dsp/TapeProcessor.h"

class OneKnobAudioProcessor : public juce::AudioProcessor
{
public:
    OneKnobAudioProcessor();
    ~OneKnobAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() noexcept { return valueTreeState; }

    static constexpr const char* kAmountParamID = "amount";

private:
    juce::AudioProcessorValueTreeState              valueTreeState;
    std::atomic<float>*                             amountParam = nullptr;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedAmount;
    oneknob::TapeProcessor                          tapeProcessor;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneKnobAudioProcessor)
};
