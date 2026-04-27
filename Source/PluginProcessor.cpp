#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobAudioProcessor::OneKnobAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                       #if ! JucePlugin_IsMidiEffect
                        #if ! JucePlugin_IsSynth
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        #endif
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       #endif
                     ),
      valueTreeState (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    amountParam = valueTreeState.getRawParameterValue (kAmountParamID);
    jassert (amountParam != nullptr);
}

OneKnobAudioProcessor::~OneKnobAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout
OneKnobAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kAmountParamID, 1 },
        "Amount",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.25f));

    return layout;
}

const juce::String OneKnobAudioProcessor::getName() const          { return JucePlugin_Name; }
bool   OneKnobAudioProcessor::acceptsMidi() const                  { return false; }
bool   OneKnobAudioProcessor::producesMidi() const                 { return false; }
bool   OneKnobAudioProcessor::isMidiEffect() const                 { return false; }
double OneKnobAudioProcessor::getTailLengthSeconds() const         { return 0.0; }

int    OneKnobAudioProcessor::getNumPrograms()                     { return 1; }
int    OneKnobAudioProcessor::getCurrentProgram()                  { return 0; }
void   OneKnobAudioProcessor::setCurrentProgram (int)              {}
const  juce::String OneKnobAudioProcessor::getProgramName (int)    { return {}; }
void   OneKnobAudioProcessor::changeProgramName (int, const juce::String&) {}

void OneKnobAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec {
        sampleRate,
        static_cast<juce::uint32> (samplesPerBlock),
        static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()))
    };

    tapeProcessor.prepare (spec);

    smoothedAmount.reset (sampleRate, 0.030);                    // 30 ms ramp
    smoothedAmount.setCurrentAndTargetValue (amountParam->load());

    setLatencySamples (tapeProcessor.getLatencySamples());
}

void OneKnobAudioProcessor::releaseResources()
{
    tapeProcessor.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneKnobAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    const auto& mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono()
     && mainOut != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (mainOut != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OneKnobAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalIn  = getTotalNumInputChannels();
    const auto totalOut = getTotalNumOutputChannels();

    for (auto ch = totalIn; ch < totalOut; ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    smoothedAmount.setTargetValue (amountParam->load());
    const float currentAmount = smoothedAmount.getCurrentValue();

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> ctx (block);
    tapeProcessor.process (ctx, currentAmount);

    smoothedAmount.skip (buffer.getNumSamples());
}

bool OneKnobAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* OneKnobAudioProcessor::createEditor()
{
    return new OneKnobAudioProcessorEditor (*this);
}

void OneKnobAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void OneKnobAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (valueTreeState.state.getType()))
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobAudioProcessor();
}
