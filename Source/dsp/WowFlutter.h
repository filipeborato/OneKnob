#pragma once

#include <JuceHeader.h>
#include <cmath>

namespace oneknob
{
    /** Subtle pitch modulation via an interpolated delay line, modulated by
        the sum of a slow Wow LFO (~0.7 Hz) and a faster Flutter LFO (~7 Hz)
        with a small amount of band-limited noise jitter.

        Wow/flutter is a transport-time phenomenon — the same modulation is
        applied to all channels (mono modulation), preserving stereo imaging.
    */
    class WowFlutter
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec)
        {
            sampleRate = static_cast<float> (spec.sampleRate);

            // Reserve enough delay for: base + max wow + max flutter (incl. jitter)
            // + safety. Hard caps are enforced in setDepths(); this just guarantees
            // we never overrun the buffer if AmountMapper limits ever change.
            const float headRoomMs = baseDelayMs
                                   + maxWowDepthMs
                                   + maxFlutterDepthMs * (1.0f + jitterRelDepth)
                                   + 1.0f;  // safety
            const auto maxDelaySamples =
                static_cast<int> (std::ceil (headRoomMs * 0.001f * sampleRate)) + 16;

            delay.reset();
            delay.setMaximumDelayInSamples (maxDelaySamples);
            delay.prepare (spec);

            baseDelaySamples = baseDelayMs * 0.001f * sampleRate;
            wowPhaseInc      = juce::MathConstants<float>::twoPi * 0.7f / sampleRate;
            flutterPhaseInc  = juce::MathConstants<float>::twoPi * 7.0f / sampleRate;

            // Sample-rate-invariant 1-pole LPF for the jitter source.
            jitterCoef = std::exp (-2.0f * juce::MathConstants<float>::pi
                                   * jitterCutoffHz / sampleRate);

            reset();
        }

        void reset() noexcept
        {
            delay.reset();
            wowPhase = flutterPhase = 0.0f;
            jitterState = 0.0f;
        }

        void setDepths (float wowMs, float flutterMs) noexcept
        {
            // Clamp to the per-stage ceilings used to size the delay buffer in
            // prepare(). Defensive: keeps us safe if AmountMapper ever changes.
            wowMs     = juce::jlimit (0.0f, maxWowDepthMs,     wowMs);
            flutterMs = juce::jlimit (0.0f, maxFlutterDepthMs, flutterMs);

            wowDepthSamples     = wowMs     * 0.001f * sampleRate;
            flutterDepthSamples = flutterMs * 0.001f * sampleRate;
        }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            auto& block = ctx.getOutputBlock();
            const auto numS  = block.getNumSamples();
            const auto numCh = juce::jmin ((size_t) 2, block.getNumChannels());

            for (size_t i = 0; i < numS; ++i)
            {
                wowPhase     += wowPhaseInc;
                flutterPhase += flutterPhaseInc;
                if (wowPhase     > juce::MathConstants<float>::twoPi) wowPhase     -= juce::MathConstants<float>::twoPi;
                if (flutterPhase > juce::MathConstants<float>::twoPi) flutterPhase -= juce::MathConstants<float>::twoPi;

                // Smoothed white noise → pseudo-jitter. 1-pole LPF coefficient
                // is computed from a fixed cutoff so the character stays the
                // same at 44.1, 48, 96 kHz, etc.
                const float rawNoise = noise.nextFloat() * 2.0f - 1.0f;
                jitterState = jitterCoef * jitterState + (1.0f - jitterCoef) * rawNoise;

                const float wowMod     = std::sin (wowPhase) * wowDepthSamples;
                const float flutterMod = (std::sin (flutterPhase) + jitterRelDepth * jitterState)
                                          * flutterDepthSamples;

                const float delaySamples = juce::jmax (0.0f,
                                            baseDelaySamples + wowMod + flutterMod);
                delay.setDelay (delaySamples);

                for (size_t ch = 0; ch < numCh; ++ch)
                {
                    auto* p = block.getChannelPointer (ch);
                    delay.pushSample (static_cast<int> (ch), p[i]);
                    p[i] = delay.popSample (static_cast<int> (ch));
                }
            }
        }

    private:
        // Hard ceilings — guarantee delay reservation never overruns regardless
        // of what AmountMapper produces. baseDelay + maxWow + maxFlutter*(1+jit) ≈ 9 ms.
        static constexpr float baseDelayMs       = 1.0f;
        static constexpr float maxWowDepthMs     = 4.0f;
        static constexpr float maxFlutterDepthMs = 4.0f;
        static constexpr float jitterRelDepth    = 0.10f;
        static constexpr float jitterCutoffHz    = 500.0f;

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delay;
        juce::Random noise;

        float sampleRate          = 44100.0f;
        float baseDelaySamples    = 0.0f;
        float wowDepthSamples     = 0.0f;
        float flutterDepthSamples = 0.0f;

        float wowPhase     = 0.0f;
        float flutterPhase = 0.0f;
        float wowPhaseInc     = 0.0f;
        float flutterPhaseInc = 0.0f;
        float jitterState = 0.0f;
        float jitterCoef  = 0.0f;
    };
}
