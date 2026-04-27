#pragma once

#include <JuceHeader.h>

namespace oneknob
{
    /** Static makeup gain. Ramps linearly across the block to avoid
        click on coefficient changes from the AmountMapper.

        v1 uses an empirical curve (positive dB compensates the RMS loss
        introduced by drive + HF roll-off). Replace with a measured LUT
        once you A/B at fixed loudness with pink noise. */
    class LoudnessComp
    {
    public:
        void prepare()             noexcept { gainCurrent = gainTarget; }
        void reset()               noexcept { gainCurrent = gainTarget; }

        void setMakeupDb (float db) noexcept
        {
            gainTarget = juce::Decibels::decibelsToGain (db);
        }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            auto& block = ctx.getOutputBlock();
            const auto numS = block.getNumSamples();
            if (numS == 0) return;

            const auto numCh = block.getNumChannels();
            const float start = gainCurrent;
            const float inc   = (gainTarget - gainCurrent) / static_cast<float> (numS);

            for (size_t ch = 0; ch < numCh; ++ch)
            {
                auto* p = block.getChannelPointer (ch);
                for (size_t i = 0; i < numS; ++i)
                    p[i] *= start + inc * static_cast<float> (i);
            }

            gainCurrent = gainTarget;
        }

    private:
        float gainTarget  = 1.0f;
        float gainCurrent = 1.0f;
    };
}
