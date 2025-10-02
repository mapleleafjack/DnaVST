#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ColorScheme
{
public:
    /**
     * Generate a color using HSV to RGB conversion.
     */
    static juce::Colour hsvToRgb(float h, float s, float v);
    
    /**
     * Generate a gradient color for DNA visualization (cyan to purple).
     * @param position Position in the gradient (0..1)
     * @param level Audio level for alpha modulation (0..1)
     */
    static juce::Colour getDNAGradientColor(float position, float level);
};
