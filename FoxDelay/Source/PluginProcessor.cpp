/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FoxDelayAudioProcessor::FoxDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FoxDelayAudioProcessor::~FoxDelayAudioProcessor()
{
}

//==============================================================================
const juce::String FoxDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FoxDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FoxDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FoxDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FoxDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FoxDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FoxDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FoxDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FoxDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void FoxDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FoxDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = sampleRate * 2; // 2 seconds
    delayBuffer.setSize(getTotalNumOutputChannels(), (int)delayBufferSize);
    delayBuffer.clear();
}

void FoxDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FoxDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void FoxDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        fillDelayBuffer(buffer, channel);
        applyDelayBuffer(buffer, channel);

    }
    writePosition +=  buffer.getNumSamples();
    writePosition %= delayBuffer.getNumSamples();
    
}

void FoxDelayAudioProcessor::fillDelayBuffer(AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    if (writePosition + bufferSize < delayBufferSize)
    {
        delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer (channel), bufferSize);
    }
    else
    {
        auto samplesAvailableAtBufferEnd = delayBufferSize - writePosition;
        delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer (channel), samplesAvailableAtBufferEnd);
        auto samplesNeededAtBufferStart = bufferSize - samplesAvailableAtBufferEnd;
        delayBuffer.copyFrom(channel, 0, buffer.getWritePointer (channel, samplesAvailableAtBufferEnd), samplesNeededAtBufferStart);
    }

}



void FoxDelayAudioProcessor::applyDelayBuffer(AudioBuffer<float>& buffer, int channel)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    int delayLengthInSamples = getSampleRate() * delayTimeInSeconds;
    auto readPosition = writePosition - delayLengthInSamples;
    if (readPosition < 0)
    {
        readPosition += delayBufferSize;
    }
    
    float dryGain = sqrt(1.0f - mixLevel);
    float wetGain = sqrt(mixLevel);

    buffer.applyGain(channel, 0, bufferSize, dryGain);
    
    if(readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, wetGain, wetGain);
        delayBuffer.addFromWithRamp(channel, writePosition, delayBuffer.getReadPointer(channel, readPosition), bufferSize, feedbackLevel, feedbackLevel);

    }
    else
    {
        auto samplesAvailableAtBufferEnd = delayBufferSize - readPosition;
        auto samplesNeededFromBufferStart = bufferSize - samplesAvailableAtBufferEnd;
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), samplesAvailableAtBufferEnd, wetGain, wetGain);
        
        delayBuffer.addFromWithRamp(channel, writePosition, delayBuffer.getReadPointer(channel, readPosition), samplesAvailableAtBufferEnd, feedbackLevel, feedbackLevel);
        
        buffer.addFromWithRamp(channel, samplesAvailableAtBufferEnd, delayBuffer.getReadPointer(channel, 0), samplesNeededFromBufferStart, wetGain, wetGain);
        
        delayBuffer.addFromWithRamp(channel, writePosition + samplesAvailableAtBufferEnd,  delayBuffer.getReadPointer(channel, 0), samplesNeededFromBufferStart, feedbackLevel, feedbackLevel);
        
    }
}

//==============================================================================
bool FoxDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FoxDelayAudioProcessor::createEditor()
{
    return new FoxDelayAudioProcessorEditor (*this);
}

//==============================================================================
void FoxDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FoxDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FoxDelayAudioProcessor();
}
