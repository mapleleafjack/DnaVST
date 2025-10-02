#include "DNA.h"

DNA::DNA(int width, int height)
    : screenWidth(width)
    , screenHeight(height)
    , backgroundColor(juce::Colours::black)
{
}

DNA::~DNA()
{
}

void DNA::render(juce::Graphics& g, float rotation, float speedMultiplier, float zoom, float heightGain, float thickness, const float* amplitudes)
{
    drawFrame(g, rotation, speedMultiplier, zoom, heightGain, thickness, amplitudes);
}

void DNA::setBackgroundColor(juce::Colour color)
{
    backgroundColor = color;
}

void DNA::setBounds(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
}

void DNA::drawFrame(juce::Graphics& g, float rotation, float speedMultiplier, float zoom, float heightGain, float thickness, const float* amplitudes)
{
    // Fill background
    g.fillAll(backgroundColor);
    
    const float centreX = screenWidth * 0.5f;
    const float centreY = screenHeight * 0.5f;
    
    // Convert rotation from degrees to radians
    const float rotRad = rotation * juce::MathConstants<float>::pi / 180.0f;
    const float cosRot = std::cos(rotRad);
    const float sinRot = std::sin(rotRad);
    
    // Update helix angle based on time
    auto currentTime = juce::Time::getMillisecondCounter();
    if (currentTime - lastTwistTime >= 50)
    {
        // Reduced increment to slow down animation and prevent overlapping appearance
        helixAngle += 0.01f * speedMultiplier;
        lastTwistTime = currentTime;
    }
    
    // Generate idle animation levels if no amplitudes provided
    static float idlePhase = 0.0f;
    idlePhase += 0.02f * speedMultiplier;
    
    // Check if we have active audio signal (any amplitude above threshold)
    bool hasActiveAudio = false;
    float maxAmplitude = 0.0f;
    if (amplitudes)
    {
        for (int i = 0; i < 42; ++i)  // NUM_BANDS from processor
        {
            maxAmplitude = std::max(maxAmplitude, amplitudes[i]);
            if (amplitudes[i] > 0.05f)  // Higher threshold to avoid triggering on silence/noise
            {
                hasActiveAudio = true;
            }
        }
    }
    
    // CORE PRINCIPLE: Fixed helix dimensions, rotation doesn't change size
    // The helix always spans the full viewport in its natural orientation
    // Zoom controls dot density, not overall size
    
    // Base dimensions - always fill the viewport
    const float helixRadius = std::min(screenWidth, screenHeight) * 0.4f;  // Radius of the helix
    const float helixLength = std::max(screenWidth, screenHeight) * 1.2f;   // Total length along the helix axis
    
    // Zoom controls the spacing between rungs (dot density)
    // Higher zoom = more dots, lower zoom = fewer dots
    // Use a minimum spacing to prevent rungs from becoming too dense
    const float baseSpacing = 25.0f;
    const float minSpacing = 3.0f;  // Prevent rungs from being closer than this
    const float rungSpacing = std::max(minSpacing, baseSpacing / zoom);
    const int numRungs = static_cast<int>(helixLength / rungSpacing);
    const int maxRungs = std::min(numRungs, MAX_RUNGS);
    
    // Draw DNA helix
    for (int i = 0; i < maxRungs; ++i)
    {
        // Position along helix axis (centered)
        float axisPos = (i - maxRungs * 0.5f) * rungSpacing;
        
        // Get amplitude level for this rung (0..1)
        float level;
        if (amplitudes && hasActiveAudio)
        {
            // Map current rung index to the amplitude array (which has 42 entries)
            // Use floating point to get smooth interpolation across all rungs
            float amplitudePos = (float)i / (float)(maxRungs - 1) * 41.0f; // 0 to 41
            int index1 = static_cast<int>(amplitudePos);
            int index2 = std::min(index1 + 1, 41);
            float fraction = amplitudePos - index1;
            
            // Interpolate between adjacent amplitude values for smooth transitions
            level = amplitudes[index1] * (1.0f - fraction) + amplitudes[index2] * fraction;
        }
        else
        {
            // Idle animation: gentle sine wave pattern when no audio or silent
            float wave1 = std::sin(idlePhase + i * 0.2f) * 0.03f;
            float wave2 = std::sin(idlePhase * 0.7f + i * 0.15f) * 0.02f;
            level = (0.15f + wave1 + wave2) * heightGain;
        }
        
        // Helix angle for this rung
        float helixPhase = helixAngle + i * 0.2f;
        
        // Calculate positions of left and right strands in helix space (before rotation)
        // In helix space: x is radial, y is along the axis
        float radiusAtRung = helixRadius * level;
        float leftHelixX = radiusAtRung * std::cos(helixPhase);
        float leftHelixY = axisPos;
        float rightHelixX = -radiusAtRung * std::cos(helixPhase);
        float rightHelixY = axisPos;
        
        // Apply rotation transform around center and translate to screen space
        float leftX = centreX + (leftHelixX * cosRot - leftHelixY * sinRot);
        float leftY = centreY + (leftHelixX * sinRot + leftHelixY * cosRot);
        float rightX = centreX + (rightHelixX * cosRot - rightHelixY * sinRot);
        float rightY = centreY + (rightHelixX * sinRot + rightHelixY * cosRot);
        
        // Generate audio-plugin style color gradient (cyan to purple)
        float colorPos = (float)i / (float)maxRungs;
        juce::Colour color = juce::Colour::fromFloatRGBA(
            0.2f + colorPos * 0.5f,  // Red: fade from cyan/blue to purple
            0.6f - colorPos * 0.4f,  // Green: bright in middle
            0.9f,                     // Blue: keep high
            0.8f + level * 0.2f);     // Alpha: slightly transparent, brighter with audio
        
        // Dot size scales with zoom and thickness parameter
        float dotSize = 3.5f * zoom * thickness * (1.0f + level * 0.4f);
        
        // Draw left strand dot with glow
        if (leftX >= -dotSize && leftX <= screenWidth + dotSize && 
            leftY >= -dotSize && leftY <= screenHeight + dotSize)
        {
            // Outer glow
            g.setColour(color.withAlpha(0.25f));
            g.fillEllipse(leftX - dotSize * 1.3f, leftY - dotSize * 1.3f, dotSize * 2.6f, dotSize * 2.6f);
            // Inner bright dot
            g.setColour(color);
            g.fillEllipse(leftX - dotSize * 0.5f, leftY - dotSize * 0.5f, dotSize, dotSize);
        }
        
        // Draw right strand dot with glow
        if (rightX >= -dotSize && rightX <= screenWidth + dotSize && 
            rightY >= -dotSize && rightY <= screenHeight + dotSize)
        {
            // Outer glow
            g.setColour(color.withAlpha(0.25f));
            g.fillEllipse(rightX - dotSize * 1.3f, rightY - dotSize * 1.3f, dotSize * 2.6f, dotSize * 2.6f);
            // Inner bright dot
            g.setColour(color);
            g.fillEllipse(rightX - dotSize * 0.5f, rightY - dotSize * 0.5f, dotSize, dotSize);
        }
        
        // Draw connecting line (rung) with thickness based on audio level and thickness parameter
        g.setColour(color);
        float lineThickness = (1.5f + level * 1.0f) * zoom * thickness;
        g.drawLine(leftX, leftY, rightX, rightY, lineThickness);
    }
}

juce::Colour DNA::hsvToRgb(float h, float s, float v) const
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
