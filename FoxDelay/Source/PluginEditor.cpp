/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FoxDelayAudioProcessorEditor::FoxDelayAudioProcessorEditor (FoxDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    mixLevelSlider.setRange(0.0f, 100.f, 1.0f);
    delayTimeSlider.setRange(0.01, 2.0f, 0.01f);
    feedbackLevelSlider.setRange(0.0f, 100.f, 1.0f);
    
    mixLevelSlider.setValue(20.0f);
    delayTimeSlider.setValue(0.5f);
    feedbackLevelSlider.setValue(30.f);
    
    setUpSlider(mixLevelSlider, "%");
    setUpSlider(delayTimeSlider, "s");
    setUpSlider(feedbackLevelSlider, "%");
    
    setUpLabel(mixLevelLabel, mixLevelSlider, "Wet Level");
    setUpLabel(delayTimeLabel, delayTimeSlider, "Delay Time");
    setUpLabel(feedbackLevelLabel, feedbackLevelSlider, "Feedback");
    
    setSize (500, 200);
}

void FoxDelayAudioProcessorEditor::setUpSlider(Slider& slider, String suffix)
{
    slider.setSliderStyle(Slider::SliderStyle::Rotary);
    slider.setSize(200, 200);
    slider.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 30);
    slider.addListener(this);
    slider.setTextValueSuffix(suffix);
    addAndMakeVisible(slider);
}

void FoxDelayAudioProcessorEditor::setUpLabel(Label& label, Slider& slider, String labelText)
{
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    label.setJustificationType(Justification::Flags::horizontallyCentred);
    addAndMakeVisible(label);
}

FoxDelayAudioProcessorEditor::~FoxDelayAudioProcessorEditor()
{
}

//==============================================================================
void FoxDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void FoxDelayAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if(slider == &mixLevelSlider)
    {
        audioProcessor.mixLevel = mixLevelSlider.getValue() /  100.0f;
    }
    else if (slider == &feedbackLevelSlider)
    {
        audioProcessor.feedbackLevel = feedbackLevelSlider.getValue() / 100.0f;
    }
    else if (slider == &delayTimeSlider)
    {
        audioProcessor.delayTimeInSeconds = delayTimeSlider.getValue();
    }
}


void FoxDelayAudioProcessorEditor::resized()
{
    mixLevelSlider.setBounds(50, 50, 100, 100);
    delayTimeSlider.setBounds(200, 50, 100, 100);
    feedbackLevelSlider.setBounds(350, 50, 100, 100);
}
