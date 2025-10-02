#include "ControlPanel.h"
#include "../Parameters/ParameterIDs.h"

ControlPanel::ControlPanel(juce::AudioProcessorValueTreeState& apvts)
{
    // Setup sliders with compact floating overlay styling
    setupSlider(heightGainSlider, heightGainLabel, "Gain");
    setupSlider(rotationSlider, rotationLabel, "Rotation");
    setupSlider(zoomSlider, zoomLabel, "Zoom");
    setupSlider(thicknessSlider, thicknessLabel, "Thickness");
    
    // Attach sliders to parameters
    heightGainAttachment = std::make_unique<SliderAttachment>(apvts, ParamIDs::heightGain, heightGainSlider);
    rotationAttachment = std::make_unique<SliderAttachment>(apvts, ParamIDs::rotation, rotationSlider);
    zoomAttachment = std::make_unique<SliderAttachment>(apvts, ParamIDs::zoom, zoomSlider);
    thicknessAttachment = std::make_unique<SliderAttachment>(apvts, ParamIDs::thickness, thicknessSlider);
    
    // Start hidden - explicitly hide all children
    setVisible(false);
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::paint(juce::Graphics& g)
{
    // Draw translucent background panel
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff1a1a1a).withAlpha(0.85f));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff3a3a3a).withAlpha(0.5f));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

void ControlPanel::resized()
{
    const int sliderHeight = 18;
    const int spacing = 3;
    const int labelWidth = 70;
    
    auto area = getLocalBounds().reduced(10);
    
    auto sliderRow = area.removeFromTop(sliderHeight);
    heightGainLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    heightGainSlider.setBounds(sliderRow);
    
    area.removeFromTop(spacing);
    sliderRow = area.removeFromTop(sliderHeight);
    rotationLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    rotationSlider.setBounds(sliderRow);
    
    area.removeFromTop(spacing);
    sliderRow = area.removeFromTop(sliderHeight);
    zoomLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    zoomSlider.setBounds(sliderRow);
    
    area.removeFromTop(spacing);
    sliderRow = area.removeFromTop(sliderHeight);
    thicknessLabel.setBounds(sliderRow.removeFromLeft(labelWidth));
    thicknessSlider.setBounds(sliderRow);
}

void ControlPanel::setVisible(bool shouldBeVisible)
{
    Component::setVisible(shouldBeVisible);
    
    heightGainSlider.setVisible(shouldBeVisible);
    rotationSlider.setVisible(shouldBeVisible);
    zoomSlider.setVisible(shouldBeVisible);
    thicknessSlider.setVisible(shouldBeVisible);
    heightGainLabel.setVisible(shouldBeVisible);
    rotationLabel.setVisible(shouldBeVisible);
    zoomLabel.setVisible(shouldBeVisible);
    thicknessLabel.setVisible(shouldBeVisible);
    
    // Also set interception for mouse events when visible
    setInterceptsMouseClicks(shouldBeVisible, shouldBeVisible);
}

juce::Rectangle<int> ControlPanel::getControlPanelBounds() const
{
    return getBounds();
}

void ControlPanel::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
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
}
