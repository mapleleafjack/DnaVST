#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    
    // Initialize DNA animation
    dnaAnimation = std::make_unique<DNA>(400, 300);
    
    // Setup parameter sliders with compact floating overlay styling
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& labelText)
    {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 42, 16);
        slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4a4a4a));
        slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff2a2a2a));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff4a9eff));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffcccccc));
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);
        
        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffbbbbbb));
        label.setFont(juce::FontOptions(10.0f));
        addAndMakeVisible(label);
    };
    
    setupSlider(heightGainSlider, heightGainLabel, "Gain");
    setupSlider(rotationSlider, rotationLabel, "Rotation");
    setupSlider(zoomSlider, zoomLabel, "Zoom");
    setupSlider(thicknessSlider, thicknessLabel, "Thickness");
    
    // Attach sliders to parameters
    heightGainAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "heightGain", heightGainSlider);
    rotationAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "rotation", rotationSlider);
    zoomAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "zoom", zoomSlider);
    thicknessAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "thickness", thicknessSlider);
    
    // Start with controls hidden
    setControlsVisible(false);
    
    // Start timer for animation updates
    startTimer(16); // ~60 FPS
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Update DNA animation to fill entire window
    dnaAnimation->setBounds(getWidth(), getHeight());
    
    // Get parameters
    float rotation = processorRef.apvts.getRawParameterValue("rotation")->load();
    float speed = 2.0f; // Fixed speed value
    float zoom = processorRef.apvts.getRawParameterValue("zoom")->load();
    float heightGain = processorRef.apvts.getRawParameterValue("heightGain")->load();
    float thickness = processorRef.apvts.getRawParameterValue("thickness")->load();
    
    // Get amplitude data from processor - always pass the current audio levels
    // The DNA animation will handle idle animation internally when levels are low
    const float* amplitudes = processorRef.getAmplitudes();
    
    // Render DNA animation
    {
        juce::Graphics::ScopedSaveState saveState(g);
        dnaAnimation->render(g, rotation, speed, zoom, heightGain, thickness, amplitudes);
    }
    
    // Draw translucent background panels for floating controls only when visible
    if (controlsVisible)
    {
        // Slider panel background
        auto sliderPanelBounds = getControlPanelBounds();
        g.setColour(juce::Colour(0xff1a1a1a).withAlpha(0.85f));
        g.fillRoundedRectangle(sliderPanelBounds.toFloat(), 6.0f);
        g.setColour(juce::Colour(0xff3a3a3a).withAlpha(0.5f));
        g.drawRoundedRectangle(sliderPanelBounds.toFloat(), 6.0f, 1.0f);
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Floating controls overlay on the visualization
    const int sliderHeight = 18;
    const int spacing = 3;
    const int labelWidth = 70;
    const int valueWidth = 42;
    const int totalSliderWidth = labelWidth + valueWidth + 100; // 100 for slider track
    
    // Position sliders in bottom-right corner as floating overlay
    int startY = getHeight() - (sliderHeight * 4 + spacing * 3 + 10);
    int startX = getWidth() - totalSliderWidth - 10;
    
    auto sliderRow = juce::Rectangle<int>(startX, startY, totalSliderWidth, sliderHeight);
    heightGainLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    heightGainSlider.setBounds(sliderRow);
    
    startY += sliderHeight + spacing;
    sliderRow = juce::Rectangle<int>(startX, startY, totalSliderWidth, sliderHeight);
    rotationLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    rotationSlider.setBounds(sliderRow);
    
    startY += sliderHeight + spacing;
    sliderRow = juce::Rectangle<int>(startX, startY, totalSliderWidth, sliderHeight);
    zoomLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    zoomSlider.setBounds(sliderRow);
    
    startY += sliderHeight + spacing;
    sliderRow = juce::Rectangle<int>(startX, startY, totalSliderWidth, sliderHeight);
    thicknessLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    thicknessSlider.setBounds(sliderRow);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Repaint to update DNA animation
    repaint();
}

void AudioPluginAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    auto clickPos = event.getPosition();
    auto controlPanelBounds = getControlPanelBounds();
    
    // If controls are visible
    if (controlsVisible)
    {
        // Check if click is inside control panel
        if (controlPanelBounds.contains(clickPos))
        {
            // Let the controls handle the click
            return;
        }
        else
        {
            // Click outside panel - prepare for potential drag
            isDraggingRotation = false; // Will be set to true in mouseDrag if they actually drag
            lastDragPosition = clickPos;
            dragStartRotation = processorRef.apvts.getRawParameterValue("rotation")->load();
        }
    }
    else
    {
        // Controls are hidden - prepare for potential drag
        isDraggingRotation = false; // Will be set to true in mouseDrag if they actually drag
        lastDragPosition = clickPos;
        dragStartRotation = processorRef.apvts.getRawParameterValue("rotation")->load();
    }
}

void AudioPluginAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
    auto currentPos = event.getPosition();
    
    // Check if we've moved enough to start dragging (avoid accidental drags on clicks)
    if (!isDraggingRotation)
    {
        int dragDistance = std::abs(currentPos.y - lastDragPosition.y);
        if (dragDistance > 3) // Threshold in pixels
        {
            isDraggingRotation = true;
            // Hide controls when starting to drag
            if (controlsVisible)
                setControlsVisible(false);
        }
        else
        {
            return; // Not enough movement yet
        }
    }
    
    if (isDraggingRotation)
    {
        // Calculate rotation based on vertical drag distance
        // Drag up = rotate clockwise, drag down = rotate counter-clockwise
        // Each pixel of vertical movement = 0.5 degrees of rotation
        float dragDelta = (lastDragPosition.y - currentPos.y) * 0.5f; // Note: reversed Y for intuitive control
        float newRotation = dragStartRotation + dragDelta;
        
        // Wrap rotation to stay within -180 to 180 range
        while (newRotation > 180.0f) newRotation -= 360.0f;
        while (newRotation < -180.0f) newRotation += 360.0f;
        
        // Update the rotation parameter
        auto* rotationParam = processorRef.apvts.getParameter("rotation");
        if (rotationParam)
        {
            float normalizedValue = (newRotation + 180.0f) / 360.0f; // Convert to 0..1 range
            rotationParam->setValueNotifyingHost(normalizedValue);
        }
    }
}

void AudioPluginAudioProcessorEditor::mouseUp(const juce::MouseEvent& event)
{
    auto clickPos = event.getPosition();
    auto controlPanelBounds = getControlPanelBounds();
    
    // If we didn't actually drag (just clicked), toggle controls visibility
    if (!isDraggingRotation)
    {
        // Don't toggle if clicking on controls themselves
        if (!controlPanelBounds.contains(clickPos))
        {
            if (controlsVisible)
            {
                setControlsVisible(false);
            }
            else
            {
                setControlsVisible(true);
            }
        }
    }
    
    isDraggingRotation = false;
}

void AudioPluginAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    juce::ignoreUnused(event);
    
    // Use scroll wheel to control zoom with increased sensitivity
    // wheel.deltaY is typically -1 to 1, scale it for better responsiveness
    float currentZoom = processorRef.apvts.getRawParameterValue("zoom")->load();
    float zoomDelta = wheel.deltaY * 0.3f; // Increased from 0.1f for more sensitivity
    
    // Also support horizontal scroll if vertical is 0
    if (wheel.deltaY == 0.0f)
        zoomDelta = wheel.deltaX * 0.3f;
    
    float newZoom = juce::jlimit(0.5f, 2.0f, currentZoom + zoomDelta);
    
    // Update the zoom parameter
    auto* zoomParam = processorRef.apvts.getParameter("zoom");
    if (zoomParam)
    {
        float normalizedValue = (newZoom - 0.5f) / 1.5f; // Convert to 0..1 range (0.5-2.0)
        zoomParam->setValueNotifyingHost(normalizedValue);
    }
}

void AudioPluginAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    auto clickPos = event.getPosition();
    auto controlPanelBounds = getControlPanelBounds();
    
    // Don't toggle if clicking on controls themselves
    if (!controlPanelBounds.contains(clickPos))
    {
        // Toggle controls visibility on double-click
        toggleControlsVisibility();
        isDraggingRotation = false; // Cancel any drag operation
    }
}

void AudioPluginAudioProcessorEditor::toggleControlsVisibility()
{
    setControlsVisible(!controlsVisible);
}

void AudioPluginAudioProcessorEditor::setControlsVisible(bool visible)
{
    controlsVisible = visible;
    
    // Show/hide all control components
    heightGainSlider.setVisible(visible);
    rotationSlider.setVisible(visible);
    zoomSlider.setVisible(visible);
    thicknessSlider.setVisible(visible);
    heightGainLabel.setVisible(visible);
    rotationLabel.setVisible(visible);
    zoomLabel.setVisible(visible);
    thicknessLabel.setVisible(visible);
    
    repaint();
}

juce::Rectangle<int> AudioPluginAudioProcessorEditor::getControlPanelBounds() const
{
    return juce::Rectangle<int>(
        getWidth() - 220, 
        getHeight() - 95, 
        215, 
        90
    );
}
