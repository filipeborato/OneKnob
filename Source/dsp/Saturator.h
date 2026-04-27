#pragma once

#include <JuceHeader.h>
#include <cmath>

namespace oneknob
{
    /** Tape-flavoured soft saturator: tanh with a small asymmetric (x²) term
        to introduce 2nd-harmonic content. Designed to run inside an oversampled
        domain — by itself it WILL alias.

        Drive ramps linearly across the block to avoid zipper noise on
        coefficient updates from the AmountMapper.
    */
    class Saturator
    {
    public:
        void prepare()       noexcept { driveCurrent = driveTarget; }
        void reset()         noexcept { driveCurrent = driveTarget; }
        void setDrive (float linear) noexcept { driveTarget = juce::jmax (1.0e-3f, linear); }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            auto& block = ctx.getOutputBlock();
            const auto numS  = block.getNumSamples();
            if (numS == 0) return;

            const auto numCh = block.getNumChannels();
            const float start = driveCurrent;
            const float inc   = (driveTarget - driveCurrent) / static_cast<float> (numS);

            for (size_t ch = 0; ch < numCh; ++ch)
            {
                auto* p = block.getChannelPointer (ch);
                for (size_t i = 0; i < numS; ++i)
                {
                    const float drive = start + inc * static_cast<float> (i);
                    const float invD  = 1.0f / drive;
                    const float y     = p[i] * drive;
                    // tanh + small asymmetry → mostly odd harmonics with subtle 2nd
                    p[i] = std::tanh (y + asymmetry * y * y) * invD;
                }
            }

            driveCurrent = driveTarget;
        }

    private:
        static constexpr float asymmetry = 0.04f;
        float driveTarget  = 1.0f;
        float driveCurrent = 1.0f;
    };
}
