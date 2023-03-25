#pragma once

#include <JuceHeader.h>

class FoxDelayBuffer
{
public:
    FoxDelayBuffer ();
    ~FoxDelayBuffer();
    
    void setDelayBufferSize(int channels, int samples, double sampleRate);
    void setReadBufferSize(int channels, int samples);
    void addToBuffer(AudioBuffer<float>& buffer, int channel, float gain, bool overwrite);
    AudioBuffer<float> readFromBuffer(int channel, float delayTimeInSeconds);
    AudioBuffer<float> readSectionFromBuffer(int channel, int startSample, int sampleLength);
    AudioBuffer<float> readFromBufferBackwards(int channel, float delayTimeInSeconds, int loopPosition);
    void clear();
    void advanceReversePosition(int samples);
    void endCycle(AudioBuffer<float>& buffer);

private:
    int writePosition { 0 };
    int readPosition { 0 };
    int reversePosition { 0 };
    double delaySampleRate;
    AudioBuffer<float> delayBuffer;
    AudioBuffer<float> readBuffer;
};
