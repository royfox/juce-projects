
#include "DelayBuffer.h"

FoxDelayBuffer::FoxDelayBuffer () {}
FoxDelayBuffer::~FoxDelayBuffer() {}

void FoxDelayBuffer::setDelayBufferSize(int channels, int samples, double sampleRate)
{
    delayBuffer.setSize(channels, samples);
    delaySampleRate = sampleRate;
}

void FoxDelayBuffer::setReadBufferSize(int channels, int samples)
{
    readBuffer.setSize(channels, samples);
    readBuffer.clear();
}

void FoxDelayBuffer::clear()
{
    delayBuffer.clear();
    readBuffer.clear();
}

void FoxDelayBuffer::addToBuffer(AudioBuffer<float>& buffer, int channel, float gain, bool overwrite)
{
    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    if (writePosition + bufferSize < delayBufferSize)
    {
        if (overwrite == true)
        {
            delayBuffer.copyFromWithRamp(channel, writePosition, buffer.getWritePointer (channel), bufferSize, gain, gain);
        }
        else
        {
            delayBuffer.addFromWithRamp(channel, writePosition, buffer.getWritePointer (channel), bufferSize, gain, gain);
        }

    }
    else
    {
        auto samplesAvailableAtBufferEnd = delayBufferSize - writePosition;
        auto samplesNeededAtBufferStart = bufferSize - samplesAvailableAtBufferEnd;
        
        if (overwrite == true)
        {
            delayBuffer.copyFromWithRamp(channel, writePosition, buffer.getWritePointer (channel), samplesAvailableAtBufferEnd, gain, gain);
            delayBuffer.copyFromWithRamp(channel, 0, buffer.getWritePointer (channel, samplesAvailableAtBufferEnd), samplesNeededAtBufferStart, gain, gain);
        }
        else
        {
            delayBuffer.addFromWithRamp(channel, writePosition, buffer.getWritePointer (channel), samplesAvailableAtBufferEnd, gain, gain);
            delayBuffer.addFromWithRamp(channel, 0, buffer.getWritePointer (channel, samplesAvailableAtBufferEnd), samplesNeededAtBufferStart, gain, gain);
        }

    }
}

void FoxDelayBuffer::endCycle(AudioBuffer<float>& buffer)
{
    writePosition +=  buffer.getNumSamples();
    writePosition %= delayBuffer.getNumSamples();
}

AudioBuffer<float> FoxDelayBuffer::readFromBuffer(int channel, float delayTimeInSeconds)
{
    auto delayBufferSize = delayBuffer.getNumSamples();
    auto delayLengthInSamples = delaySampleRate * delayTimeInSeconds;
    auto readPosition = writePosition - delayLengthInSamples;
    if (readPosition < 0)
    {
        readPosition += delayBufferSize;
    }
    return readSectionFromBuffer(channel, readPosition, delayLengthInSamples);
}

AudioBuffer<float> FoxDelayBuffer::readSectionFromBuffer(int channel, int readPosition, int delayLengthInSamples) {
    auto readBufferSize = readBuffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    if(readPosition + readBufferSize < delayBufferSize)
    {
        readBuffer.copyFrom(channel, 0, delayBuffer.getReadPointer(channel, readPosition), readBufferSize);
    }
    else
    {
        auto samplesAvailableAtBufferEnd = delayBufferSize - readPosition;
        auto samplesNeededFromBufferStart = readBufferSize - samplesAvailableAtBufferEnd;
        readBuffer.copyFrom(channel, 0, delayBuffer.getReadPointer(channel, readPosition), samplesAvailableAtBufferEnd);
        
        readBuffer.copyFrom(channel, samplesAvailableAtBufferEnd, delayBuffer.getReadPointer(channel, 0), samplesNeededFromBufferStart);
        
    }
    return readBuffer;
}

AudioBuffer<float> FoxDelayBuffer::readFromBufferBackwards(int channel, float delayTimeInSeconds, int loopSamplePosition)
{
    int delayLengthInSamples = delaySampleRate * delayTimeInSeconds;
    auto readBufferSize = readBuffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();
    
    auto reverseReadPosition = reversePosition - loopSamplePosition;
    if (reverseReadPosition < 0) {
        reverseReadPosition += delayBufferSize;
    }
    auto buffer = readSectionFromBuffer(channel, reverseReadPosition, delayLengthInSamples);
    buffer.reverse(channel, 0, readBufferSize);
    return buffer;

}

void FoxDelayBuffer::advanceReversePosition(int samples)
{
    auto delayBufferSize = delayBuffer.getNumSamples();
    reversePosition += samples;
    if (reversePosition > delayBufferSize) {
        reversePosition = reversePosition % delayBufferSize;
    }
}
