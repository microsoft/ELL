////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetectorNode.tcc (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <utilities/include/Unused.h>

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    VoiceActivityDetectorNode<ValueType>::VoiceActivityDetectorNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1)
    {
    }

    template <typename ValueType>
    VoiceActivityDetectorNode<ValueType>::VoiceActivityDetectorNode(const model::OutputPort<ValueType>& input,
                                                                    double sampleRate,
                                                                    double frameDuration,
                                                                    double tauUp,
                                                                    double tauDown,
                                                                    double largeInput,
                                                                    double gainAtt,
                                                                    double thresholdUp,
                                                                    double thresholdDown,
                                                                    double levelThreshold) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _vad(sampleRate, input.Size(), frameDuration, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold)
    {
    }
} // namespace nodes
} // namespace ell
