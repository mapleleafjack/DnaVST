#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class AudioAnalyzer
{
public:
    static constexpr int NUM_BANDS = 32;
    
    AudioAnalyzer();
    ~AudioAnalyzer();
    
    /**
     * Analyzes an audio buffer and updates the amplitude bands.
     * @param buffer The audio buffer to analyze
     * @param heightGain Gain multiplier for the amplitude values
     */
    void analyzeBuffer(const juce::AudioBuffer<float>& buffer, float heightGain);
    
    /**
     * Get the current amplitude values (smoothed).
     * @return Pointer to array of NUM_BANDS amplitude values
     */
    const float* getAmplitudes() const { return smoothedAmplitudes; }
    
    /**
     * Reset all amplitude values to zero.
     */
    void reset();
    
    /**
     * Set smoothing coefficients for attack and release.
     * @param attack Attack smoothing (0..1, higher = slower)
     * @param release Release smoothing (0..1, higher = slower)
     */
    void setSmoothingCoefficients(float attack, float release);

private:
    float amplitudes[NUM_BANDS];
    float smoothedAmplitudes[NUM_BANDS];
    
    float attackSmoothing = 0.3f;
    float releaseSmoothing = 0.7f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioAnalyzer)
};
