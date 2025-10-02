#include "DNARenderer.h"

DNARenderer::DNARenderer()
    : backgroundColor(juce::Colours::black)
{
}

DNARenderer::~DNARenderer()
{
}

void DNARenderer::setBackgroundColor(juce::Colour color)
{
    backgroundColor = color;
}

void DNARenderer::render(juce::Graphics& g,
                         juce::Rectangle<int> bounds,
                         float helixAngle,
                         float rotation,
                         float zoom,
                         float heightGain,
                         float thickness,
                         const float* amplitudes,
                         int numAmplitudes,
                         float idlePhase)
{
    // Fill background
    g.fillAll(backgroundColor);
    
    const float centreX = bounds.getWidth() * 0.5f;
    const float centreY = bounds.getHeight() * 0.5f;
    
    // Convert rotation from degrees to radians
    const float rotRad = rotation * juce::MathConstants<float>::pi / 180.0f;
    const float cosRot = std::cos(rotRad);
    const float sinRot = std::sin(rotRad);
    
    // Check if we have active audio signal
    bool hasActiveAudio = false;
    if (amplitudes)
    {
        for (int i = 0; i < numAmplitudes; ++i)
        {
            if (amplitudes[i] > 0.05f)
            {
                hasActiveAudio = true;
                break;
            }
        }
    }
    
    // Base dimensions - always fill the viewport
    const float helixRadius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.4f;
    const float helixLength = std::max(bounds.getWidth(), bounds.getHeight()) * 1.2f;
    
    // Zoom controls the spacing between rungs
    const float baseSpacing = 25.0f;
    const float minSpacing = 3.0f;
    const float rungSpacing = std::max(minSpacing, baseSpacing / zoom);
    const int numRungs = static_cast<int>(helixLength / rungSpacing);
    const int maxRungs = std::min(numRungs, MAX_RUNGS);
    
    // Draw DNA helix
    for (int i = 0; i < maxRungs; ++i)
    {
        // Position along helix axis (centered)
        float axisPos = (i - maxRungs * 0.5f) * rungSpacing;
        
        // Get amplitude level for this rung
        float level;
        if (amplitudes && hasActiveAudio)
        {
            // Map current rung to amplitude array with interpolation
            float amplitudePos = (float)i / (float)(maxRungs - 1) * (numAmplitudes - 1);
            int index1 = static_cast<int>(amplitudePos);
            int index2 = std::min(index1 + 1, numAmplitudes - 1);
            float fraction = amplitudePos - index1;
            
            level = amplitudes[index1] * (1.0f - fraction) + amplitudes[index2] * fraction;
        }
        else
        {
            // Idle animation: gentle sine wave pattern
            float wave1 = std::sin(idlePhase + i * 0.2f) * 0.03f;
            float wave2 = std::sin(idlePhase * 0.7f + i * 0.15f) * 0.02f;
            level = (0.15f + wave1 + wave2) * heightGain;
        }
        
        // Helix phase for this rung
        float helixPhase = helixAngle + i * 0.2f;
        
        // Calculate positions in helix space
        float radiusAtRung = helixRadius * level;
        float leftHelixX = radiusAtRung * std::cos(helixPhase);
        float leftHelixY = axisPos;
        float rightHelixX = -radiusAtRung * std::cos(helixPhase);
        float rightHelixY = axisPos;
        
        // Apply rotation and translate to screen space
        HelixPoint left, right;
        left.x = centreX + (leftHelixX * cosRot - leftHelixY * sinRot);
        left.y = centreY + (leftHelixX * sinRot + leftHelixY * cosRot);
        left.level = level;
        
        right.x = centreX + (rightHelixX * cosRot - rightHelixY * sinRot);
        right.y = centreY + (rightHelixX * sinRot + rightHelixY * cosRot);
        right.level = level;
        
        // Generate color
        float colorPos = (float)i / (float)maxRungs;
        juce::Colour color = ColorScheme::getDNAGradientColor(colorPos, level);
        
        // Dot size scales with zoom and thickness
        float dotSize = 3.5f * zoom * thickness * (1.0f + level * 0.4f);
        
        // Draw strands and rung
        drawStrand(g, left, color, dotSize);
        drawStrand(g, right, color, dotSize);
        drawRung(g, left, right, color, (1.5f + level * 1.0f) * zoom * thickness);
    }
}

void DNARenderer::drawStrand(juce::Graphics& g,
                             const HelixPoint& point,
                             const juce::Colour& color,
                             float dotSize)
{
    // Outer glow
    g.setColour(color.withAlpha(0.25f));
    g.fillEllipse(point.x - dotSize * 1.3f, point.y - dotSize * 1.3f, 
                  dotSize * 2.6f, dotSize * 2.6f);
    
    // Inner bright dot
    g.setColour(color);
    g.fillEllipse(point.x - dotSize * 0.5f, point.y - dotSize * 0.5f, 
                  dotSize, dotSize);
}

void DNARenderer::drawRung(juce::Graphics& g,
                           const HelixPoint& left,
                           const HelixPoint& right,
                           const juce::Colour& color,
                           float thickness)
{
    g.setColour(color);
    g.drawLine(left.x, left.y, right.x, right.y, thickness);
}
