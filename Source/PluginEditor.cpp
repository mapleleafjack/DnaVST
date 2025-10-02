#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/ParameterIDs.h"
#include "Parameters/ParameterDefaults.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), controlPanel(p.apvts)
{
    juce::ignoreUnused (processorRef);
    
    // Add visualizer - don't intercept mouse clicks so we can handle drag/click in editor
    addAndMakeVisible(visualizer);
    visualizer.setInterceptsMouseClicks(false, false);
    
    // Add control panel on top (but keep it hidden initially)
    addChildComponent(controlPanel); // Use addChildComponent instead of addAndMakeVisible
    controlPanel.setVisible(false);  // Explicitly start hidden
    
    // Start timer for parameter updates
    startTimer(16); // ~60 FPS
    
    setSize (600, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background is painted by visualizer
    g.fillAll(juce::Colour(0xff1a1a1a));
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Visualizer fills entire window
    visualizer.setBounds(getLocalBounds());
    
    // Control panel in bottom-right corner - increased size for all parameters
    const int panelWidth = 230;
    const int panelHeight = 105;
    const int margin = 10;
    
    controlPanel.setBounds(
        getWidth() - panelWidth - margin,
        getHeight() - panelHeight - margin,
        panelWidth,
        panelHeight
    );
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Update visualizer parameters from APVTS
    visualizer.setRotation(processorRef.apvts.getRawParameterValue(ParamIDs::rotation)->load());
    visualizer.setZoom(processorRef.apvts.getRawParameterValue(ParamIDs::zoom)->load());
    visualizer.setHeightGain(processorRef.apvts.getRawParameterValue(ParamIDs::heightGain)->load());
    visualizer.setThickness(processorRef.apvts.getRawParameterValue(ParamIDs::thickness)->load());
    visualizer.setSpeedMultiplier(2.0f); // Fixed speed
    
    // Update amplitude data
    visualizer.setAmplitudes(processorRef.getAmplitudes(), processorRef.NUM_BANDS);
}

void AudioPluginAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    auto clickPos = event.getPosition();
    auto controlPanelBounds = controlPanel.getBounds();
    
    // Prepare for potential drag (unless clicking on control panel)
    if (controlPanel.isVisible() && controlPanelBounds.contains(clickPos))
    {
        isDraggingRotation = false;
        return; // Let controls handle the click
    }
    
    // Store initial state for drag
    isDraggingRotation = false;
    lastDragPosition = clickPos;
    dragStartRotation = processorRef.apvts.getRawParameterValue(ParamIDs::rotation)->load();
}

void AudioPluginAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
    auto currentPos = event.getPosition();
    
    // Check if we've moved enough to start dragging
    if (!isDraggingRotation)
    {
        int dragDistance = std::abs(currentPos.y - lastDragPosition.y);
        if (dragDistance > 3)
            isDraggingRotation = true;
        else
            return;
    }
    
    if (isDraggingRotation)
    {
        // Calculate rotation based on vertical drag
        float dragDelta = (lastDragPosition.y - currentPos.y) * 0.5f;
        float newRotation = dragStartRotation + dragDelta;
        
        // Wrap rotation to stay within -180 to 180 range
        while (newRotation > 180.0f) newRotation -= 360.0f;
        while (newRotation < -180.0f) newRotation += 360.0f;
        
        // Update the rotation parameter
        auto* rotationParam = processorRef.apvts.getParameter(ParamIDs::rotation);
        if (rotationParam)
        {
            float normalizedValue = (newRotation + 180.0f) / 360.0f;
            rotationParam->setValueNotifyingHost(normalizedValue);
        }
    }
}

void AudioPluginAudioProcessorEditor::mouseUp(const juce::MouseEvent& event)
{
    auto clickPos = event.getPosition();
    auto controlPanelBounds = controlPanel.getBounds();
    
    // If we didn't drag, toggle controls visibility
    if (!isDraggingRotation && !controlPanelBounds.contains(clickPos))
    {
        toggleControlsVisibility();
    }
    
    isDraggingRotation = false;
}

void AudioPluginAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    juce::ignoreUnused(event);
    
    // Use scroll wheel to control zoom
    float currentZoom = processorRef.apvts.getRawParameterValue(ParamIDs::zoom)->load();
    float zoomDelta = wheel.deltaY * 0.3f;
    
    if (wheel.deltaY == 0.0f)
        zoomDelta = wheel.deltaX * 0.3f;
    
    float newZoom = juce::jlimit(ParamDefaults::zoomMin, ParamDefaults::zoomMax, currentZoom + zoomDelta);
    
    auto* zoomParam = processorRef.apvts.getParameter(ParamIDs::zoom);
    if (zoomParam)
    {
        float normalizedValue = (newZoom - ParamDefaults::zoomMin) / (ParamDefaults::zoomMax - ParamDefaults::zoomMin);
        zoomParam->setValueNotifyingHost(normalizedValue);
    }
}

void AudioPluginAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    auto clickPos = event.getPosition();
    auto controlPanelBounds = controlPanel.getBounds();
    
    if (!controlPanelBounds.contains(clickPos))
    {
        toggleControlsVisibility();
        isDraggingRotation = false;
    }
}

void AudioPluginAudioProcessorEditor::toggleControlsVisibility()
{
    bool newVisibility = !controlPanel.isVisible();
    controlPanel.setVisible(newVisibility);
    if (newVisibility)
        controlPanel.toFront(false);
}
