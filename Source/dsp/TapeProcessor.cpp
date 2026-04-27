#include "TapeProcessor.h"

namespace oneknob
{
    static constexpr size_t kOversampleFactorLog2 = 2;   // log2(4) = 2 → 4× OS

    void TapeProcessor::prepare (const juce::dsp::ProcessSpec& spec)
    {
        dcBlocker.prepare (spec.sampleRate);
        wowFlutter.prepare (spec);
        tiltEq.prepare (spec.sampleRate);
        postLpf.prepare (spec.sampleRate);
        saturator.prepare();
        loudnessComp.prepare();

        oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
            static_cast<size_t> (spec.numChannels),
            kOversampleFactorLog2,
            juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
            true,    // maximum quality
            false    // non-integer latency is fine
        );
        oversampling->initProcessing (static_cast<size_t> (spec.maximumBlockSize));

        latencySamples = static_cast<int> (std::ceil (oversampling->getLatencyInSamples()));

        reset();
    }

    void TapeProcessor::reset()
    {
        dcBlocker.reset();
        wowFlutter.reset();
        tiltEq.reset();
        saturator.reset();
        postLpf.reset();
        loudnessComp.reset();
        if (oversampling) oversampling->reset();
    }

    void TapeProcessor::process (juce::dsp::ProcessContextReplacing<float>& ctx, float amount)
    {
        const auto coeffs = mapAmount (amount);

        // Push the new targets into each stage. Drive + makeup ramp internally.
        wowFlutter.setDepths   (coeffs.wowDepthMs, coeffs.flutterDepthMs);
        tiltEq.setShelves      (coeffs.lowShelfDb, coeffs.highShelfDb);
        saturator.setDrive     (coeffs.driveLin);
        postLpf.setCutoff      (coeffs.postLpfHz);
        loudnessComp.setMakeupDb (coeffs.makeupDb);

        // Stage 1 — base rate (linear).
        dcBlocker.process (ctx);
        wowFlutter.process (ctx);
        tiltEq.process    (ctx);

        // Stage 2 — oversampled domain (saturator only — that is what aliases).
        auto& outBlock = ctx.getOutputBlock();
        if (oversampling)
        {
            juce::dsp::AudioBlock<const float> constBlock (outBlock);
            auto upBlock = oversampling->processSamplesUp (constBlock);
            juce::dsp::ProcessContextReplacing<float> upCtx (upBlock);
            saturator.process (upCtx);
            oversampling->processSamplesDown (outBlock);
        }

        // Stage 3 — base rate again.
        postLpf.process      (ctx);
        loudnessComp.process (ctx);
    }
}
