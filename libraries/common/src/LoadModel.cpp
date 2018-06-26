////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadModel.cpp (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadModel.h"

// model
#include "Map.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "BroadcastFunctionNode.h"
#include "BufferNode.h"
#include "ClockNode.h"
#include "ConcatenationNode.h"
#include "DCTNode.h"
#include "DiagonalConvolutionNode.h"
#include "ClockNode.h"
#include "CompiledActivationFunctions.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "DTWDistanceNode.h"
#include "ExtremalValueNode.h"
#include "FFTNode.h"
#include "ForestPredictorNode.h"
#include "HammingWindowNode.h"
#include "IIRFilterNode.h"
#include "L2NormSquaredNode.h"
#include "LinearPredictorNode.h"
#include "FilterBankNode.h"
#include "MatrixMatrixMultiplyNode.h"
#include "MatrixVectorProductNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "MultiplexerNode.h"
#include "NeuralNetworkPredictorNode.h"
#include "ProtoNNPredictorNode.h"
#include "ReceptiveFieldMatrixNode.h"
#include "ReorderDataNode.h"
#include "SimpleConvolutionNode.h"
#include "SinkNode.h"
#include "SourceNode.h"
#include "UnaryOperationNode.h"
#include "UnrolledConvolutionNode.h"
#include "VoiceActivityDetectorNode.h"
#include "WinogradConvolutionNode.h"

// predictors
#include "ForestPredictor.h"
#include "LinearPredictor.h"
#include "NeuralNetworkPredictor.h"
#include "SingleElementThresholdPredictor.h"

// utilities
#include "Archiver.h"
#include "Files.h"
#include "JsonArchiver.h"

// stl
#include <cstdint>

using namespace ell::predictors::neural;

namespace ell
{
namespace common
{
    template<typename ElementType>
    void RegisterRealNodeTypes(utilities::SerializationContext& context)
    {
        context.GetTypeFactory().AddType<model::Node, model::InputNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryOperationNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<ElementType, nodes::HardSigmoidActivationFunction<ElementType>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<ElementType, nodes::LeakyReLUActivationFunction<ElementType>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<ElementType, nodes::ReLUActivationFunction<ElementType>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<ElementType, nodes::SigmoidActivationFunction<ElementType>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastLinearFunctionNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConcatenationNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DiagonalConvolutionNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DotProductNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DTWDistanceNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::FFTNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::HammingWindowNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::L2NormSquaredNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::IIRFilterNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LinearPredictorNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LinearFilterBankNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MelFilterBankNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixVectorProductNode<ElementType, math::MatrixLayout::rowMajor>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixVectorProductNode<ElementType, math::MatrixLayout::columnMajor>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixMatrixMultiplyNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingAverageNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingVarianceNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::NeuralNetworkPredictorNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ReceptiveFieldMatrixNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ReorderDataNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SimpleConvolutionNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SinkNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SourceNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::UnaryOperationNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::UnrolledConvolutionNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::VoiceActivityDetectorNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::WinogradConvolutionNode<ElementType>>();

        // NN layer nodes
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<ElementType, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<ElementType, LeakyReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<ElementType, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<ElementType, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<ElementType, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BatchNormalizationLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BiasLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryConvolutionalLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConvolutionalLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::FullyConnectedLayerNode<ElementType>>();

        //
        // GRULayerNode with inner product of ['TanhActivation', 'SigmoidActivation', 'HardSigmoidActivation', 'ReLUActivation']
        //
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, TanhActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, TanhActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, TanhActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, TanhActivation, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, SigmoidActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, SigmoidActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, SigmoidActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, SigmoidActivation, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, HardSigmoidActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, HardSigmoidActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, HardSigmoidActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, HardSigmoidActivation, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, ReLUActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, ReLUActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, ReLUActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::GRULayerNode<ElementType, ReLUActivation, ReLUActivation>>();

        //
        // LSTMLayer with inner product of ['TanhActivation', 'SigmoidActivation', 'HardSigmoidActivation', 'ReLUActivation']
        //
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, TanhActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, TanhActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, TanhActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, TanhActivation, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, SigmoidActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, SigmoidActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, SigmoidActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, SigmoidActivation, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, HardSigmoidActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, HardSigmoidActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, HardSigmoidActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, HardSigmoidActivation, ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, ReLUActivation, TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, ReLUActivation, SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, ReLUActivation, HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LSTMLayerNode<ElementType, ReLUActivation, ReLUActivation>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ParametricReLUActivationLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::PoolingLayerNode<ElementType, MeanPoolingFunction>>();
        context.GetTypeFactory().AddType<model::Node, nodes::PoolingLayerNode<ElementType, MaxPoolingFunction>>();
        context.GetTypeFactory().AddType<model::Node, nodes::RegionDetectionLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ScalingLayerNode<ElementType>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SoftmaxLayerNode<ElementType>>();

    }

    void RegisterNodeTypes(utilities::SerializationContext& context)
    {
        RegisterRealNodeTypes<float>(context);
        RegisterRealNodeTypes<double>(context);

        // additional non-real types, only for nodes that support that.
        context.GetTypeFactory().AddType<model::Node, model::InputNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<int>>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, model::OutputNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<int>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastLinearFunctionNode<int>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ClockNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::ConcatenationNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConcatenationNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConcatenationNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DCTNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DCTNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DemultiplexerNode<bool, bool>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<bool, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<int, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<int64_t, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<float, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<double, bool>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ProtoNNPredictorNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::SimpleForestPredictorNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::SingleElementThresholdNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, int64_t>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, int64_t>>();

    }

    void RegisterMapTypes(utilities::SerializationContext& context)
    {
        context.GetTypeFactory().AddType<model::Map, model::Map>();
    }

    template <typename UnarchiverType>
    model::Model LoadArchivedModel(std::istream& stream)
    {
        utilities::SerializationContext context;
        RegisterNodeTypes(context);
        UnarchiverType unarchiver(stream, context);
        model::Model model;
        unarchiver.Unarchive(model);
        return model;
    }

    template <typename ArchiverType, typename ObjectType>
    void SaveArchivedObject(const ObjectType& obj, std::ostream& stream)
    {
        ArchiverType archiver(stream);
        archiver.Archive(obj);
    }

    model::Model LoadModel(const std::string& filename)
    {
        if (!utilities::IsFileReadable(filename))
        {
            throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound);
        }

        auto filestream = utilities::OpenIfstream(filename);
        return LoadArchivedModel<utilities::JsonUnarchiver>(filestream);
    }

    void SaveModel(const model::Model& model, const std::string& filename)
    {
        if (!utilities::IsFileWritable(filename))
        {
            throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotWritable);
        }
        auto filestream = utilities::OpenOfstream(filename);
        SaveModel(model, filestream);
    }

    void SaveModel(const model::Model& model, std::ostream& outStream)
    {
        SaveArchivedObject<utilities::JsonArchiver>(model, outStream);
    }

    //
    // Map
    //
    model::Map LoadMap(const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.HasMapFilename())
        {
            return common::LoadMap(mapLoadArguments.inputMapFilename);
        }
        else if (mapLoadArguments.HasModelFilename())
        {
            auto model = common::LoadModel(mapLoadArguments.inputModelFilename);

            model::InputNodeBase* inputNode = nullptr;
            model::PortElementsBase outputElements;
            if (mapLoadArguments.modelInputsString != "")
            {
                inputNode = mapLoadArguments.GetInput(model);
                if (inputNode == nullptr)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find input node");
                }
            }
            else // look for first input node
            {
                auto inputNodes = model.GetNodesByType<model::InputNodeBase>();
                if (inputNodes.size() == 0)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find input node");
                }
                inputNode = inputNodes[0];
            }

            if (mapLoadArguments.modelOutputsString != "")
            {
                outputElements = mapLoadArguments.GetOutput(model);
            }
            else // look for first output node
            {
                auto outputNodes = model.GetNodesByType<model::OutputNodeBase>();
                if (outputNodes.size() == 0)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find output node");
                }
                auto outputNode = outputNodes[0];
                auto outputPorts = outputNode->GetOutputPorts();
                if (outputPorts.size() == 0)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Can't find output port");
                }

                auto outputPort = outputPorts[0]; // ptr to port base
                outputElements = model::PortElementsBase(*outputPort);
            }

            return { model, { { "input", inputNode } }, { { "output", outputElements } } };
        }
        else // No model / map file specified -- return an identity map
        {
            model::Model model;
            auto inputNode = model.AddNode<model::InputNode<double>>(mapLoadArguments.defaultInputSize);
            model::PortElements<double> outputElements(inputNode->output);

            return { model, { { "input", inputNode } }, { { "output", outputElements } } };
        }
    }

    model::Map LoadMap(const std::string& filename)
    {
        if (filename == "")
        {
            return model::Map{};
        }

        if (!utilities::IsFileReadable(filename))
        {
            throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound);
        }

        auto filestream = utilities::OpenIfstream(filename);
        return LoadArchivedMap<utilities::JsonUnarchiver>(filestream);
    }

    void SaveMap(const model::Map& map, const std::string& filename)
    {
        if (!utilities::IsFileWritable(filename))
        {
            throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotWritable);
        }
        auto filestream = utilities::OpenOfstream(filename);
        SaveMap(map, filestream);
    }

    void SaveMap(const model::Map& map, std::ostream& outStream)
    {
        SaveArchivedObject<utilities::JsonArchiver>(map, outStream);
    }
}
}
