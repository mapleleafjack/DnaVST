#include "ColorScheme.h"

juce::Colour ColorScheme::hsvToRgb(float h, float s, float v)
{
    float r, g, b;
    int i = static_cast<int>(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    
    switch (i % 6)
    {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }
    
    return juce::Colour::fromFloatRGBA(r, g, b, 1.0f);
}

juce::Colour ColorScheme::getDNAGradientColor(float position, float level)
{
    // Audio-plugin style gradient (cyan to purple)
    return juce::Colour::fromFloatRGBA(
        0.2f + position * 0.5f,  // Red: fade from cyan/blue to purple
        0.6f - position * 0.4f,  // Green: bright in middle
        0.9f,                     // Blue: keep high
        0.8f + level * 0.2f);     // Alpha: slightly transparent, brighter with audio
}
