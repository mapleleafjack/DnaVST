#include "VisualizerComponent.h"

VisualizerComponent::VisualizerComponent()
{
    // Initialize amplitude data
    for (int i = 0; i < MAX_AMPLITUDE_BANDS; ++i)
        amplitudeData[i] = 0.0f;
    
    startTimer(16); // ~60 FPS
}

VisualizerComponent::~VisualizerComponent()
{
    stopTimer();
}

void VisualizerComponent::paint(juce::Graphics& g)
{
    renderer.render(g,
                    getLocalBounds(),
                    helixAngle,
                    rotation,
                    zoom,
                    heightGain,
                    thickness,
                    numAmplitudeBands > 0 ? amplitudeData : nullptr,
                    numAmplitudeBands,
                    idlePhase);
}

void VisualizerComponent::resized()
{
    // Nothing to do - rendering adapts to bounds automatically
}

void VisualizerComponent::timerCallback()
{
    auto currentTime = juce::Time::getMillisecondCounter();
    
    // Update helix angle
    if (currentTime - lastUpdateTime >= 50)
    {
        helixAngle += 0.01f * speedMultiplier;
        lastUpdateTime = currentTime;
    }
    
    // Update idle animation phase
    idlePhase += 0.02f * speedMultiplier;
    
    repaint();
}

void VisualizerComponent::setRotation(float degrees)
{
    rotation = degrees;
}

void VisualizerComponent::setZoom(float zoomLevel)
{
    zoom = zoomLevel;
}

void VisualizerComponent::setHeightGain(float gain)
{
    heightGain = gain;
}

void VisualizerComponent::setThickness(float thick)
{
    thickness = thick;
}

void VisualizerComponent::setSpeedMultiplier(float speed)
{
    speedMultiplier = speed;
}

void VisualizerComponent::setAmplitudes(const float* amplitudes, int numAmplitudes)
{
    if (amplitudes == nullptr || numAmplitudes == 0)
    {
        numAmplitudeBands = 0;
        return;
    }
    
    numAmplitudeBands = juce::jmin(numAmplitudes, MAX_AMPLITUDE_BANDS);
    
    for (int i = 0; i < numAmplitudeBands; ++i)
        amplitudeData[i] = amplitudes[i];
}

void VisualizerComponent::setBackgroundColor(juce::Colour color)
{
    renderer.setBackgroundColor(color);
}
