#pragma once

#include "PluginProcessor.h"
#include "DNA.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;

private:
    void toggleControlsVisibility();
    void setControlsVisible(bool visible);
    juce::Rectangle<int> getControlPanelBounds() const;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    
    // DNA Animation
    std::unique_ptr<DNA> dnaAnimation;
    
    // Parameter attachments for real-time updates
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    juce::Slider heightGainSlider;
    juce::Slider rotationSlider;
    juce::Slider zoomSlider;
    juce::Slider thicknessSlider;
    
    juce::Label heightGainLabel;
    juce::Label rotationLabel;
    juce::Label zoomLabel;
    juce::Label thicknessLabel;
    
    std::unique_ptr<SliderAttachment> heightGainAttachment;
    std::unique_ptr<SliderAttachment> rotationAttachment;
    std::unique_ptr<SliderAttachment> zoomAttachment;
    std::unique_ptr<SliderAttachment> thicknessAttachment;
    
    bool controlsVisible = false;
    
    // Mouse interaction tracking
    bool isDraggingRotation = false;
    juce::Point<int> lastDragPosition;
    float dragStartRotation = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
