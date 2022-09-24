
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
FoxDelayAudioProcessorEditor::FoxDelayAudioProcessorEditor (FoxDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    mixLevelSlider.setRange(0.0f, 100.f, 1.0f);
    delayTimeSlider.setRange(-6, 0, 1);
    feedbackLevelSlider.setRange(0.0f, 100.f, 1.0f);
    
    mixLevelSlider.setValue(20.0f);
    delayTimeSlider.setValue(-2);
    feedbackLevelSlider.setValue(30.f);
    
    setUpSlider(mixLevelSlider, "%");
    setUpSlider(delayTimeSlider, "");
    setUpSlider(feedbackLevelSlider, "%");
    
    delayTimeSlider.textFromValueFunction = [](double value)
    {
        float denominator = pow(2, -value);
        return "1/" + juce::String(static_cast<int>(denominator));
    };
    delayTimeSlider.updateText();
    
    
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
        float denominator = pow(2, -delayTimeSlider.getValue());
        audioProcessor.delayFraction = 1 / denominator;
    }
}


void FoxDelayAudioProcessorEditor::resized()
{
    mixLevelSlider.setBounds(50, 50, 100, 100);
    delayTimeSlider.setBounds(200, 50, 100, 100);
    feedbackLevelSlider.setBounds(350, 50, 100, 100);
}
