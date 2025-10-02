#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // DNA parameters matching the ESP32 project's VisSetting structure
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("heightGain", 1),
        "Gain",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f),
        3.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("rotation", 1),
        "Rotation",
        juce::NormalisableRange<float>(-180.0f, 180.0f, 0.1f),
        90.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("zoom", 1),
        "Zoom",
        juce::NormalisableRange<float>(0.5f, 2.0f, 0.01f),
        1.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("thickness", 1),
        "Thickness",
        juce::NormalisableRange<float>(0.5f, 3.0f, 0.01f),
        1.0f));
    
    return layout;
}

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
     , apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize amplitudes
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        amplitudes[i] = 0.0f;
        smoothedAmplitudes[i] = 0.0f;
    }
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (newSampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is a visualization-only plugin - just pass audio through
    // and analyze it for the visualization
    
    // Update amplitude analysis for visualization
    updateAmplitudes(buffer);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
void AudioPluginAudioProcessor::updateAmplitudes(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    if (numSamples == 0 || numChannels == 0)
        return;
    
    // Simple amplitude analysis - divide audio into bands
    const int samplesPerBand = juce::jmax(1, numSamples / NUM_BANDS);
    const float heightGain = apvts.getRawParameterValue("heightGain")->load();
    
    for (int band = 0; band < NUM_BANDS; ++band)
    {
        float sum = 0.0f;
        int startSample = band * samplesPerBand;
        int endSample = juce::jmin(startSample + samplesPerBand, numSamples);
        
        // Calculate RMS for this band
        for (int ch = 0; ch < numChannels; ++ch)
        {
            for (int i = startSample; i < endSample; ++i)
            {
                float sample = buffer.getSample(ch, i);
                sum += sample * sample;
            }
        }
        
        float rms = std::sqrt(sum / (float)((endSample - startSample) * numChannels));
        amplitudes[band] = rms * heightGain;
        
        // Adaptive smoothing: faster decay when signal is low to prevent imprinting
        // Slower smoothing for attack (going up), faster for release (going down)
        float targetValue = amplitudes[band];
        if (targetValue > smoothedAmplitudes[band])
        {
            // Attack - smooth rise
            const float attackSmoothing = 0.3f;
            smoothedAmplitudes[band] = smoothedAmplitudes[band] * attackSmoothing + targetValue * (1.0f - attackSmoothing);
        }
        else
        {
            // Release - faster decay to prevent imprinting
            const float releaseSmoothing = 0.7f;
            smoothedAmplitudes[band] = smoothedAmplitudes[band] * releaseSmoothing + targetValue * (1.0f - releaseSmoothing);
        }
        
        // Clamp to 0..1 range
        smoothedAmplitudes[band] = juce::jlimit(0.0f, 1.0f, smoothedAmplitudes[band]);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
