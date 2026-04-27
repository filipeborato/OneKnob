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

            // Base delay (1 ms) + worst-case modulation depth + interpolation slack.
            const auto maxDelaySamples =
                static_cast<int> (std::ceil (0.005f * sampleRate)) + 8;

            delay.reset();
            delay.setMaximumDelayInSamples (maxDelaySamples);
            delay.prepare (spec);

            baseDelaySamples = 0.001f * sampleRate;          // 1 ms
            wowPhaseInc      = juce::MathConstants<float>::twoPi * 0.7f / sampleRate;
            flutterPhaseInc  = juce::MathConstants<float>::twoPi * 7.0f / sampleRate;

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
            wowDepthSamples     = juce::jmax (0.0f, wowMs)     * 0.001f * sampleRate;
            flutterDepthSamples = juce::jmax (0.0f, flutterMs) * 0.001f * sampleRate;
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

                // Smoothed white noise → pseudo-jitter. Cheap 1-pole LPF on rng.
                const float rawNoise = noise.nextFloat() * 2.0f - 1.0f;
                jitterState = 0.92f * jitterState + 0.08f * rawNoise;

                const float wowMod     = std::sin (wowPhase) * wowDepthSamples;
                const float flutterMod = (std::sin (flutterPhase) + 0.10f * jitterState)
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
    };
}
