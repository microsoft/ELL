////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VoiceActivityDetectorNode.cpp (nodes)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VoiceActivityDetectorNode.h"

#include <emitters/include/EmitterException.h>
#include <emitters/include/EmitterTypes.h>
#include <emitters/include/IREmitter.h>
#include <emitters/include/IRLocalValue.h>

namespace ell
{
namespace nodes
{
    VoiceActivityDetectorNode::VoiceActivityDetectorNode() :
        CompilableCodeNode("VoiceActivityDetector", { &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, ell::model::Port::PortType::integer, 1)
    {
    }

    VoiceActivityDetectorNode::VoiceActivityDetectorNode(const model::OutputPortBase& input,
                                                         double sampleRate,
                                                         double frameDuration,
                                                         double tauUp,
                                                         double tauDown,
                                                         double largeInput,
                                                         double gainAtt,
                                                         double thresholdUp,
                                                         double thresholdDown,
                                                         double levelThreshold) :
        CompilableCodeNode("VoiceActivityDetector", { &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, ell::model::Port::PortType::integer, 1),
        _vad(sampleRate, input.Size(), frameDuration, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold)
    {
    }

    void VoiceActivityDetectorNode::Define(value::FunctionDeclaration& fn)
    {
        (void)fn.Define([this](value::Vector data, value::Vector output) {
            output[0] = _vad.Process(data);
        });
    }

    void VoiceActivityDetectorNode::DefineReset(value::FunctionDeclaration& fn)
    {
        (void)fn.Define([this] { _vad.Reset(); });
    }

    void VoiceActivityDetectorNode::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<VoiceActivityDetectorNode>(newPortElements, _vad.GetSampleRate(), _vad.GetFrameDuration(), _vad.GetTauUp(), _vad.GetTauDown(), _vad.GetLargeInput(), _vad.GetGainAtt(), _vad.GetThresholdUp(), _vad.GetThresholdDown(), _vad.GetLevelThreshold());
        transformer.MapNodeOutput(output, newNode->output);
    }

    void VoiceActivityDetectorNode::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableCodeNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["vad"] << _vad;
    }

    void VoiceActivityDetectorNode::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableCodeNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["vad"] >> _vad;
    }

    const model::OutputPortBase& VoiceActivityDetector(const model::OutputPortBase& input,
                                                       double sampleRate,
                                                       double frameDuration,
                                                       double tauUp,
                                                       double tauDown,
                                                       double largeInput,
                                                       double gainAtt,
                                                       double thresholdUp,
                                                       double thresholdDown,
                                                       double levelThreshold)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }
        auto node = model->AddNode<VoiceActivityDetectorNode>(input, sampleRate, frameDuration, tauUp, tauDown, largeInput, gainAtt, thresholdUp, thresholdDown, levelThreshold);
        return node->output;
    }
} // namespace nodes
} // namespace ell
