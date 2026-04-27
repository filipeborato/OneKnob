#pragma once

#include <JuceHeader.h>
#include <cmath>

namespace oneknob
{
    /** 1st-order high-pass / DC blocker. Cutoff scales with sample rate. */
    class DcBlocker
    {
    public:
        void prepare (double sampleRate, float cutoffHz = 10.0f) noexcept
        {
            const float fc = juce::jmax (1.0f, cutoffHz);
            radius = juce::jlimit (0.0f, 0.99999f,
                                   1.0f - 2.0f * juce::MathConstants<float>::pi
                                          * fc / static_cast<float> (sampleRate));
            reset();
        }

        void reset() noexcept
        {
            for (auto& v : x1) v = 0.0f;
            for (auto& v : y1) v = 0.0f;
        }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            auto& block = ctx.getOutputBlock();
            const auto numCh = juce::jmin ((size_t) maxChannels, block.getNumChannels());
            const auto numS  = block.getNumSamples();

            for (size_t ch = 0; ch < numCh; ++ch)
            {
                auto* p = block.getChannelPointer (ch);
                float prevX = x1[ch];
                float prevY = y1[ch];

                for (size_t i = 0; i < numS; ++i)
                {
                    const float in = p[i];
                    const float out = in - prevX + radius * prevY;
                    prevX = in;
                    prevY = out;
                    p[i]  = out;
                }

                x1[ch] = prevX;
                y1[ch] = prevY;
            }
        }

    private:
        static constexpr int maxChannels = 2;
        float radius = 0.997f;
        float x1[maxChannels] {};
        float y1[maxChannels] {};
    };

    /** 1-pole low-pass filter (6 dB/oct). Used post-saturation. */
    class OnePoleLpf
    {
    public:
        void prepare (double sr) noexcept
        {
            sampleRate = static_cast<float> (sr);
            setCutoff (22000.0f);
            reset();
        }

        void reset() noexcept { for (auto& v : z) v = 0.0f; }

        void setCutoff (float fc) noexcept
        {
            fc    = juce::jlimit (1.0f, 0.49f * sampleRate, fc);
            coef  = std::exp (-2.0f * juce::MathConstants<float>::pi * fc / sampleRate);
            inv   = 1.0f - coef;
        }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            auto& block = ctx.getOutputBlock();
            const auto numCh = juce::jmin ((size_t) maxChannels, block.getNumChannels());
            const auto numS  = block.getNumSamples();

            for (size_t ch = 0; ch < numCh; ++ch)
            {
                auto* p = block.getChannelPointer (ch);
                float state = z[ch];
                for (size_t i = 0; i < numS; ++i)
                {
                    state = inv * p[i] + coef * state;
                    p[i] = state;
                }
                z[ch] = state;
            }
        }

    private:
        static constexpr int maxChannels = 2;
        float sampleRate = 44100.0f;
        float coef = 0.0f;
        float inv  = 1.0f;
        float z[maxChannels] {};
    };

    /** 1-pole shelving filter — Low or High. */
    class OnePoleShelf
    {
    public:
        enum class Type { Low, High };

        void prepare (double sr) noexcept
        {
            sampleRate = static_cast<float> (sr);
            reset();
        }

        void reset() noexcept { for (auto& v : z) v = 0.0f; }

        void set (Type t, float cutoffHz, float gainDb) noexcept
        {
            type = t;
            const float fc = juce::jlimit (1.0f, 0.49f * sampleRate, cutoffHz);
            coef = std::exp (-2.0f * juce::MathConstants<float>::pi * fc / sampleRate);
            inv  = 1.0f - coef;
            shelfGain = juce::Decibels::decibelsToGain (gainDb) - 1.0f;
        }

        template <typename Ctx>
        void process (const Ctx& ctx) noexcept
        {
            auto& block = ctx.getOutputBlock();
            const auto numCh = juce::jmin ((size_t) maxChannels, block.getNumChannels());
            const auto numS  = block.getNumSamples();

            for (size_t ch = 0; ch < numCh; ++ch)
            {
                auto* p = block.getChannelPointer (ch);
                float state = z[ch];

                if (type == Type::Low)
                {
                    for (size_t i = 0; i < numS; ++i)
                    {
                        const float in = p[i];
                        state = inv * in + coef * state;          // 1-pole LPF
                        p[i]  = in + shelfGain * state;           // bypass + scaled LF
                    }
                }
                else
                {
                    for (size_t i = 0; i < numS; ++i)
                    {
                        const float in = p[i];
                        state = inv * in + coef * state;          // 1-pole LPF
                        p[i]  = in + shelfGain * (in - state);    // bypass + scaled HF
                    }
                }

                z[ch] = state;
            }
        }

    private:
        static constexpr int maxChannels = 2;
        Type  type       = Type::Low;
        float sampleRate = 44100.0f;
        float coef       = 0.0f;
        float inv        = 1.0f;
        float shelfGain  = 0.0f;
        float z[maxChannels] {};
    };
}
