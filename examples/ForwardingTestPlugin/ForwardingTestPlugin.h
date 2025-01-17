#pragma once

#include <chowdsp_plugin_base/chowdsp_plugin_base.h>

#include "../SignalGenerator/SignalGeneratorPlugin.h"
#include "../SimpleReverb/SimpleReverbPlugin.h"
#include "InternalPlugins/WernerFilterPlugin.h"

class ForwardingTestPlugin : public chowdsp::PluginBase<ForwardingTestPlugin>,
                             private juce::AudioProcessorValueTreeState::Listener
{
public:
    ForwardingTestPlugin();
    ~ForwardingTestPlugin() override;

    static void addParameters (Parameters& params);

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processAudioBlock (juce::AudioBuffer<float>& buffer) override;

    juce::AudioProcessorEditor* createEditor() override;

    auto& getVTS() { return vts; }
    juce::AudioProcessor* getProcessorForIndex (int index);

    inline static const juce::String processorChoiceParamID = "proc_choice";

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    chowdsp::ChoiceParameter* processorChoiceParameter = nullptr;

    SignalGeneratorPlugin toneGenerator;
    SimpleReverbPlugin reverb;
    WernerFilterPlugin filter;

    struct ParamForwardingProvider
    {
        static chowdsp::ParameterID getForwardingParameterID (int paramIndex)
        {
            return { "forward_param_" + juce::String (paramIndex), 100 };
        }
    };
    using ForwardingParams = chowdsp::ForwardingParametersManager<ParamForwardingProvider, 10>;
    ForwardingParams forwardingParameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ForwardingTestPlugin)
};
