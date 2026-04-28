#pragma once

#include <JuceHeader.h>
#include <memory>

#include "AmountMapper.h"
#include "OnePoleFilters.h"
#include "TiltEQ.h"
#include "WowFlutter.h"
#include "Saturator.h"
#include "LoudnessComp.h"

namespace oneknob
{
    /** Top-level tape pipeline driven by a single normalised Amount value.

        Pipeline (stereo):
            DC blocker → WowFlutter → TiltEQ
                       → upsample 4× → Saturator → downsample
                       → Post LPF → Loudness comp

        Only the saturator runs in the oversampled domain — that is where
        new harmonics are created and where aliasing would occur.
    */
    class TapeProcessor
    {
    public:
        TapeProcessor() = default;

        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset();

        /** Processes the block in place and applies coefficient changes
            derived from amount. amount must be in [0, 1]. */
        void process (juce::dsp::ProcessContextReplacing<float>& ctx, float amount);

        /** Reported once at prepare time, equals the oversampler latency. */
        int  getLatencySamples() const noexcept { return latencySamples; }

    private:
        DcBlocker      dcBlocker;
        WowFlutter     wowFlutter;
        TiltEQ         tiltEq;
        Saturator      saturator;
        OnePoleLpf     postLpf;
        LoudnessComp   loudnessComp;

        std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;

        int latencySamples = 0;
    };
}
