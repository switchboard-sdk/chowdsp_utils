#include "chowdsp_BypassProcessor.h"

namespace chowdsp
{
template <typename SampleType, typename DelayInterpType>
void BypassProcessor<SampleType, DelayInterpType>::prepare (const juce::dsp::ProcessSpec& spec, bool onOffParam)
{
    prevOnOffParam = onOffParam;
    fadeBuffer.setMaxSize ((int) spec.numChannels, (int) spec.maximumBlockSize);

    compDelay.prepare (spec); // sample rate does not matter
}

template <typename SampleType, typename DelayInterpType>
void BypassProcessor<SampleType, DelayInterpType>::setLatencySamples (int delaySamples)
{
    setLatencySamplesInternal ((SampleType) delaySamples);
}

template <typename SampleType, typename DelayInterpType>
void BypassProcessor<SampleType, DelayInterpType>::setLatencySamples (SampleType delaySamples)
{
    static_assert (! std::is_same_v<DelayInterpType, DelayLineInterpolationTypes::None>, "Attempting to set non-integer latency value without using delay interpolation!");
    setLatencySamplesInternal (delaySamples);
}

template <typename SampleType, typename DelayInterpType>
void BypassProcessor<SampleType, DelayInterpType>::setLatencySamplesInternal (SampleType delaySamples)
{
    if (delaySamples == prevDelay)
        return;

    compDelay.setDelay ((SampleType) delaySamples);

    if (delaySamples == 0)
        compDelay.reset();

    prevDelay = delaySamples;
}

template <typename SampleType, typename DelayInterpType>
bool BypassProcessor<SampleType, DelayInterpType>::processBlockIn (const chowdsp::BufferView<SampleType>& block, bool onOffParam)
{
    enum class DelayOp
    {
        Pop,
        Push,
        Toss,
    };

    auto doDelayOp = [] (auto& sampleBuffer, auto& delay, DelayOp op) {
        if (delay.getDelay() == SampleType (0))
            return;

        for (int ch = 0; ch < sampleBuffer.getNumChannels(); ++ch)
        {
            if (op == DelayOp::Push)
            {
                auto* x = sampleBuffer.getReadPointer (ch);
                for (int n = 0; n < sampleBuffer.getNumSamples(); ++n)
                    delay.pushSample (ch, x[n]);
            }
            else if (op == DelayOp::Pop)
            {
                auto* x = sampleBuffer.getWritePointer (ch);
                for (int n = 0; n < sampleBuffer.getNumSamples(); ++n)
                    x[n] = delay.popSample (ch);
            }
            else if (op == DelayOp::Toss)
            {
                for (int n = 0; n < sampleBuffer.getNumSamples(); ++n)
                    delay.incrementReadPointer (ch);
            }
        }
    };

    doDelayOp (block, compDelay, DelayOp::Push);

    if (! onOffParam && ! prevOnOffParam)
    {
        doDelayOp (block, compDelay, DelayOp::Pop);
        return false;
    }

    if (onOffParam != prevOnOffParam)
    {
        fadeBuffer.setCurrentSize (block.getNumChannels(), block.getNumSamples());
        BufferMath::copyBufferData (block, fadeBuffer);
        doDelayOp (fadeBuffer, compDelay, DelayOp::Pop);

        if (onOffParam && latencySampleCount < 0)
            latencySampleCount = (int) compDelay.getDelay();
    }
    else
    {
        doDelayOp (block, compDelay, DelayOp::Toss);
    }

    return true;
}

template <typename SampleType, typename DelayInterpType>
void BypassProcessor<SampleType, DelayInterpType>::processBlockOut (const chowdsp::BufferView<SampleType>& block, bool onOffParam)
{
    auto fadeOutputBuffer = [onOffParam] (auto* blockPtr, const auto* fadePtr, const int startSample, const int numSamples) {
        SampleType startGain = ! onOffParam ? static_cast<SampleType> (1) // fade out
                                            : static_cast<SampleType> (0); // fade in
        SampleType endGain = static_cast<SampleType> (1) - startGain;

        SampleType gain = startGain;
        SampleType increment = (endGain - startGain) / (SampleType) (numSamples - startSample);

        juce::FloatVectorOperations::multiply (blockPtr, gain, startSample);
        juce::FloatVectorOperations::addWithMultiply (blockPtr, fadePtr, static_cast<SampleType> (1) - gain, startSample);

        for (int n = startSample; n < numSamples; ++n)
        {
            blockPtr[n] = blockPtr[n] * gain + fadePtr[n] * (static_cast<SampleType> (1) - gain);
            gain += increment;
        }
    };

    if (onOffParam == prevOnOffParam)
    {
        latencySampleCount = 0;
        return;
    }

    const auto numChannels = block.getNumChannels();
    const auto numSamples = block.getNumSamples();
    const auto startSample = getFadeStartSample (numSamples);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* blockPtr = block.getWritePointer (ch);
        const auto* fadePtr = fadeBuffer.getReadPointer (ch);

        fadeOutputBuffer (blockPtr, fadePtr, startSample, numSamples);
    }

    if (startSample < numSamples)
        prevOnOffParam = onOffParam;
}

template <typename SampleType, typename DelayInterpType>
int BypassProcessor<SampleType, DelayInterpType>::getFadeStartSample (const int numSamples)
{
    if (latencySampleCount <= 0)
    {
        latencySampleCount = -1;
        return 0;
    }

    if (latencySampleCount < numSamples / 4) // small offset is okay, just adjust the fade start
    {
        int startSample = latencySampleCount;
        latencySampleCount = -1;
        return startSample;
    }
    else // wait for latency time to catch up
    {
        latencySampleCount = juce::jmax (latencySampleCount - numSamples, 0);
        return numSamples;
    }
}

} // namespace chowdsp
