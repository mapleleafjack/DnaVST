#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Self-contained floating control panel for visualization parameters.
 */
class ControlPanel : public juce::Component
{
public:
    ControlPanel(juce::AudioProcessorValueTreeState& apvts);
    ~ControlPanel() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    /**
     * Show or hide the control panel.
     */
    void setVisible(bool shouldBeVisible) override;
    
    /**
     * Get the bounds of the control panel (for hit testing).
     */
    juce::Rectangle<int> getControlPanelBounds() const;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlPanel)
};
