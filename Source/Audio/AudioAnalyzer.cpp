#include "AudioAnalyzer.h"

AudioAnalyzer::AudioAnalyzer()
{
    reset();
}

AudioAnalyzer::~AudioAnalyzer()
{
}

void AudioAnalyzer::analyzeBuffer(const juce::AudioBuffer<float>& buffer, float heightGain)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    if (numSamples == 0 || numChannels == 0)
        return;
    
    // Divide audio into frequency bands
    const int samplesPerBand = juce::jmax(1, numSamples / NUM_BANDS);
    
    for (int band = 0; band < NUM_BANDS; ++band)
    {
        float sum = 0.0f;
        int startSample = band * samplesPerBand;
        int endSample = juce::jmin(startSample + samplesPerBand, numSamples);
        
        // Calculate RMS for this band
        for (int ch = 0; ch < numChannels; ++ch)
        {
            for (int i = startSample; i < endSample; ++i)
            {
                float sample = buffer.getSample(ch, i);
                sum += sample * sample;
            }
        }
        
        float rms = std::sqrt(sum / (float)((endSample - startSample) * numChannels));
        amplitudes[band] = rms * heightGain;
        
        // Adaptive smoothing: faster for release (going down), slower for attack (going up)
        float targetValue = amplitudes[band];
        if (targetValue > smoothedAmplitudes[band])
        {
            // Attack - smooth rise
            smoothedAmplitudes[band] = smoothedAmplitudes[band] * attackSmoothing + 
                                       targetValue * (1.0f - attackSmoothing);
        }
        else
        {
            // Release - faster decay to prevent imprinting
            smoothedAmplitudes[band] = smoothedAmplitudes[band] * releaseSmoothing + 
                                       targetValue * (1.0f - releaseSmoothing);
        }
        
        // Clamp to 0..1 range
        smoothedAmplitudes[band] = juce::jlimit(0.0f, 1.0f, smoothedAmplitudes[band]);
    }
}

void AudioAnalyzer::reset()
{
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        amplitudes[i] = 0.0f;
        smoothedAmplitudes[i] = 0.0f;
    }
}

void AudioAnalyzer::setSmoothingCoefficients(float attack, float release)
{
    attackSmoothing = juce::jlimit(0.0f, 1.0f, attack);
    releaseSmoothing = juce::jlimit(0.0f, 1.0f, release);
}
