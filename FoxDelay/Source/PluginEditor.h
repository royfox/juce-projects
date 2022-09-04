/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class FoxDelayAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public Slider::Listener
{
public:
    FoxDelayAudioProcessorEditor (FoxDelayAudioProcessor&);
    ~FoxDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(Slider* slider) override;
    void setUpSlider(Slider& slider, String suffix);
    void setUpLabel(Label& label, Slider& slider, String labelText);

private:
    Slider mixLevelSlider;
    Label mixLevelLabel;
    
    Slider delayTimeSlider;
    Label delayTimeLabel;
    
    Slider feedbackLevelSlider;
    Label feedbackLevelLabel;
    
    FoxDelayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoxDelayAudioProcessorEditor)
};
