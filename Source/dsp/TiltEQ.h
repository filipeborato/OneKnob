#pragma once

#include <JuceHeader.h>
#include "OnePoleFilters.h"

namespace oneknob
{
    /** Two-band tilt EQ: low shelf @ 200 Hz + high shelf @ 8 kHz.
        Both 1-pole (6 dB/oct) — gentler than biquad shelves and
        avoids any allocation for coefficient updates. */
    class TiltEQ
    {
    public:
        void prepare (double sampleRate) noexcept
        {
            lowShelf.prepare (sampleRate);
            highShelf.prepare (sampleRate);
            lowShelf.set  (OnePoleShelf::Type::Low,  lowFreqHz,  0.0f);
            highShelf.set (OnePoleShelf::Type::High, highFreqHz, 0.0f);
        }

        void reset() noexcept
        {
            lowShelf.reset();
            highShelf.reset();
        }

        void setShelves (float lowGainDb, float highGainDb) noexcept
        {
            lowShelf.set  (OnePoleShelf::Type::Low,  lowFreqHz,  lowGainDb);
            highShelf.set (OnePoleShelf::Type::High, highFreqHz, highGainDb);
        }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            lowShelf.process (ctx);
            highShelf.process (ctx);
        }

    private:
        static constexpr float lowFreqHz  = 200.0f;
        static constexpr float highFreqHz = 8000.0f;
        OnePoleShelf lowShelf, highShelf;
    };
}
