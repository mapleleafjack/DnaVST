#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ColorScheme.h"
#include <cmath>

constexpr int MAX_RUNGS = 200;

/**
 * Pure rendering class for DNA helix visualization.
 * Stateless - all animation state is passed in via render parameters.
 */
class DNARenderer
{
public:
    DNARenderer();
    ~DNARenderer();
    
    /**
     * Render a single frame of the DNA helix.
     * @param g Graphics context to draw into
     * @param bounds Rendering area
     * @param helixAngle Current helix animation phase
     * @param rotation Rotation angle in degrees
     * @param zoom Zoom level (dot density)
     * @param heightGain Amplitude multiplier
     * @param thickness Line and dot thickness multiplier
     * @param amplitudes Array of amplitude values (can be nullptr for idle animation)
     * @param numAmplitudes Number of elements in amplitudes array
     * @param idlePhase Phase for idle animation
     */
    void render(juce::Graphics& g,
                juce::Rectangle<int> bounds,
                float helixAngle,
                float rotation,
                float zoom,
                float heightGain,
                float thickness,
                const float* amplitudes,
                int numAmplitudes,
                float idlePhase);
    
    void setBackgroundColor(juce::Colour color);

private:
    juce::Colour backgroundColor;
    
    struct HelixPoint
    {
        float x, y;
        float level;
    };
    
    void drawStrand(juce::Graphics& g,
                    const HelixPoint& point,
                    const juce::Colour& color,
                    float dotSize);
    
    void drawRung(juce::Graphics& g,
                  const HelixPoint& left,
                  const HelixPoint& right,
                  const juce::Colour& color,
                  float thickness);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DNARenderer)
};
