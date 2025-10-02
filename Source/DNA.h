#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>

constexpr int MAX_RUNGS = 200;

class DNA
{
public:
    DNA(int width, int height);
    ~DNA();
    
    void render(juce::Graphics& g, 
                float rotation = 0.0f,
                float speedMultiplier = 1.0f,
                float zoom = 1.0f,
                float heightGain = 1.0f,
                float thickness = 1.0f,
                const float* amplitudes = nullptr);
    
    void setBackgroundColor(juce::Colour color);
    void setBounds(int width, int height);

private:
    int screenWidth;
    int screenHeight;
    juce::Colour backgroundColor;
    
    float helixAngle = 0.0f;
    juce::int64 lastTwistTime = 0;
    
    void drawFrame(juce::Graphics& g, 
                   float rotation,
                   float speedMultiplier,
                   float zoom,
                   float heightGain,
                   float thickness,
                   const float* amplitudes);
    
    juce::Colour hsvToRgb(float h, float s, float v) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DNA)
};
