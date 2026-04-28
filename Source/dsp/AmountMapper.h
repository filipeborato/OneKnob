#pragma once

#include <JuceHeader.h>

namespace oneknob
{
    struct TapeCoefficients
    {
        float driveLin;        // Saturator pre-gain (linear)        : 1.0  → 4.0
        float lowShelfDb;      // Tilt EQ low shelf @ 200 Hz         : 0.0  → +1.5 dB
        float highShelfDb;     // Tilt EQ high shelf @ 8 kHz         : 0.0  → -3.5 dB
        float postLpfHz;       // 1-pole LPF after saturator         : 22 k → 12 k
        float wowDepthMs;      // Wow modulation depth (peak)        : 0.0  → 0.30 ms
        float flutterDepthMs;  // Flutter modulation depth (peak)    : 0.0  → 0.15 ms
        float makeupDb;        // Output makeup to keep loudness     : 0.0  → +2.5 dB
    };

    /** Maps a normalised Amount in [0,1] to a coherent set of tape coefficients.

        Curves are intentionally independent: drive grows linearly, modulation
        depth grows quadratically (so wow/flutter only "open up" near the top),
        post-LPF closes linearly. Tweak by ear; do not symmetrise.
    */
    inline TapeCoefficients mapAmount (float amount) noexcept
    {
        amount = juce::jlimit (0.0f, 1.0f, amount);
        const float a   = amount;
        const float aSq = a * a;

        return {
            /* driveLin       */ 1.0f + 3.0f * a,
            /* lowShelfDb     */ 1.5f * a,
            /* highShelfDb    */ -3.5f * a,
            /* postLpfHz      */ juce::jmap (a, 22000.0f, 12000.0f),
            /* wowDepthMs     */ 0.30f * aSq,
            /* flutterDepthMs */ 0.15f * aSq,
            /* makeupDb       */ 2.5f * a
        };
    }
}
