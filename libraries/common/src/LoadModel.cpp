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
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "BroadcastFunctionNode.h"
#include "BufferNode.h"
#include "ClockNode.h"
#include "DCTNode.h"
#include "ClockNode.h"
#include "CompiledActivationFunctions.h"
#include "DelayNode.h"
#include "DotProductNode.h"
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
#include "SinkNode.h"
#include "SourceNode.h"
#include "UnaryOperationNode.h"

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

namespace ell
{
namespace common
{
    void RegisterNodeTypes(utilities::SerializationContext& context)
    {
        context.GetTypeFactory().AddType<model::Node, model::InputNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<int>>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<float>>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<double>>();

        context.GetTypeFactory().AddType<model::Node, model::OutputNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<int>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<float>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMaxNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ArgMinNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BinaryOperationNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryOperationNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastLinearFunctionNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastLinearFunctionNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastLinearFunctionNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<float, nodes::HardSigmoidActivationFunction<float>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<double, nodes::HardSigmoidActivationFunction<double>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<float, nodes::LeakyReLUActivationFunction<float>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<double, nodes::LeakyReLUActivationFunction<double>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<float, nodes::ReLUActivationFunction<float>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<double, nodes::ReLUActivationFunction<double>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<float, nodes::SigmoidActivationFunction<float>>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BroadcastUnaryFunctionNode<double, nodes::SigmoidActivationFunction<double>>>();

        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BufferNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ClockNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DCTNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DCTNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DemultiplexerNode<bool, bool>>();

        context.GetTypeFactory().AddType<model::Node, nodes::DotProductNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DotProductNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::FFTNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::FFTNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::HammingWindowNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::HammingWindowNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::L2NormSquaredNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::L2NormSquaredNode<float>>();

        context.GetTypeFactory().AddType<model::Node, nodes::IIRFilterNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::IIRFilterNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::LinearPredictorNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LinearPredictorNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::LinearFilterBankNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LinearFilterBankNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MelFilterBankNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MelFilterBankNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MatrixVectorProductNode<float, math::MatrixLayout::rowMajor>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixVectorProductNode<float, math::MatrixLayout::columnMajor>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixVectorProductNode<double, math::MatrixLayout::rowMajor>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixVectorProductNode<double, math::MatrixLayout::columnMajor>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MatrixMatrixMultiplyNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MatrixMatrixMultiplyNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<bool, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<int, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<int64_t, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<float, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<double, bool>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MovingAverageNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingAverageNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::MovingVarianceNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingVarianceNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::NeuralNetworkPredictorNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::NeuralNetworkPredictorNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ProtoNNPredictorNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::ReceptiveFieldMatrixNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ReceptiveFieldMatrixNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::ReorderDataNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ReorderDataNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::SimpleForestPredictorNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::SingleElementThresholdNode>();

        context.GetTypeFactory().AddType<model::Node, nodes::SinkNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SinkNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::SourceNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SourceNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int, double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<int64_t, double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<float, double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, int64_t>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<double, double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::UnaryOperationNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::UnaryOperationNode<double>>();

        // NN layer nodes
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<float, ell::predictors::neural::HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<double, ell::predictors::neural::HardSigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<float, ell::predictors::neural::LeakyReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<double, ell::predictors::neural::LeakyReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<float, ell::predictors::neural::ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<double, ell::predictors::neural::ReLUActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<float, ell::predictors::neural::SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<double, ell::predictors::neural::SigmoidActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<float, ell::predictors::neural::TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ActivationLayerNode<double, ell::predictors::neural::TanhActivation>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ParametricReLUActivationLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ParametricReLUActivationLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BatchNormalizationLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BatchNormalizationLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BiasLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BiasLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryConvolutionalLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryConvolutionalLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConvolutionalLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConvolutionalLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::FullyConnectedLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::FullyConnectedLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::PoolingLayerNode<float, ell::predictors::neural::MeanPoolingFunction>>();
        context.GetTypeFactory().AddType<model::Node, nodes::PoolingLayerNode<double, ell::predictors::neural::MeanPoolingFunction>>();
        context.GetTypeFactory().AddType<model::Node, nodes::PoolingLayerNode<float, ell::predictors::neural::MaxPoolingFunction>>();
        context.GetTypeFactory().AddType<model::Node, nodes::PoolingLayerNode<double, ell::predictors::neural::MaxPoolingFunction>>();
        context.GetTypeFactory().AddType<model::Node, nodes::RegionDetectionLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::RegionDetectionLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ScalingLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ScalingLayerNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SoftmaxLayerNode<float>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SoftmaxLayerNode<double>>();
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
