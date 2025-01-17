#pragma once

namespace chowdsp
{
/**
 * Useful class for managing groups of ForwardingParameter
 *
 * @tparam Provider                     Template provider type, which must provide static
 *                                      `juce::String getForwardingParameterID()` method.
 * @tparam totalNumForwardingParameters The total number of parameters to be created and managed by this class.
 */
template <typename Provider, int totalNumForwardingParameters>
class ForwardingParametersManager
{
public:
    /** Initializes handles to the forwarding parameters, and connects them to the given processor */
    explicit ForwardingParametersManager (juce::AudioProcessorValueTreeState& vts)
    {
        for (int i = 0; i < totalNumForwardingParameters; ++i)
        {
            const auto id = Provider::getForwardingParameterID (i);
#if JUCE_VERSION < 0x070000
            auto* vtsParameter = vts.getParameter (id);
#else
            auto* vtsParameter = vts.getParameter (id.getParamID());
#endif

            // this parameter was not initialized properly!
            jassert (vtsParameter != nullptr);

            auto* forwardedParam = reinterpret_cast<chowdsp::ForwardingParameter*> (vtsParameter);
            forwardedParam->setProcessor (&vts.processor);
            forwardedParams[(size_t) i] = forwardedParam;
        }
    }

    ~ForwardingParametersManager()
    {
        for (auto* param : forwardedParams)
            param->setParam (nullptr);
    }

    /** Creates the forwarding parameters using information from the templated Provider class */
    static void addParameters (chowdsp::Parameters& params)
    {
        using namespace ParamUtils;
        for (int i = 0; i < totalNumForwardingParameters; ++i)
        {
            const auto id = Provider::getForwardingParameterID (i);
            emplace_param<ForwardingParameter> (params, id, nullptr, "Blank");
        }
    }

    /** Returns a flat array of the forwarded parameters */
    [[maybe_unused]] auto& getForwardedParameters() { return forwardedParams; }

    /** Returns a flat array of the forwarded parameters */
    [[maybe_unused]] const auto& getForwardedParameters() const { return forwardedParams; }

protected:
    std::array<ForwardingParameter*, (size_t) totalNumForwardingParameters> forwardedParams;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ForwardingParametersManager)
};
} // namespace chowdsp
