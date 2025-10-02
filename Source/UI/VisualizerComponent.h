#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Visualization/DNARenderer.h"

/**
 * Component that displays the DNA helix visualization.
 * Manages animation state and timing.
 */
class VisualizerComponent : public juce::Component,
                            private juce::Timer
{
public:
    VisualizerComponent();
    ~VisualizerComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Set the visualization parameters.
     */
    void setRotation(float degrees);
    void setZoom(float zoomLevel);
    void setHeightGain(float gain);
    void setThickness(float thick);
    void setSpeedMultiplier(float speed);
    
    /**
     * Update the amplitude data for visualization.
     * @param amplitudes Array of amplitude values
     * @param numAmplitudes Size of the array
     */
    void setAmplitudes(const float* amplitudes, int numAmplitudes);
    
    /**
     * Set the background color.
     */
    void setBackgroundColor(juce::Colour color);

private:
    void timerCallback() override;
    
    DNARenderer renderer;
    
    // Animation state
    float helixAngle = 0.0f;
    float idlePhase = 0.0f;
    juce::int64 lastUpdateTime = 0;
    
    // Visualization parameters
    float rotation = 90.0f;
    float zoom = 1.5f;
    float heightGain = 3.5f;
    float thickness = 1.0f;
    float speedMultiplier = 1.0f;
    
    // Amplitude data
    static constexpr int MAX_AMPLITUDE_BANDS = 64;
    float amplitudeData[MAX_AMPLITUDE_BANDS];
    int numAmplitudeBands = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VisualizerComponent)
};
