////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryOperationNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

namespace nodes
{
    template <typename ValueType>
    UnaryOperationNode<ValueType>::UnaryOperationNode(const model::OutputPortElementList<ValueType>& input, OperationType operation) : Node({ &_input }, { &_output }), _input(this, input), _output(this, _input.Size()), _operation(operation)
    {
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> UnaryOperationNode<ValueType>::ComputeOutput(Operation&& fn) const
    {
        auto output = std::vector<ValueType>(_input.Size());
        for (size_t index = 0; index < _input.Size(); index++)
        {
            output[index] = fn(_input[index]);
        }
        return output;
    }

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        switch (_operation)
        {
            case OperationType::sqrt:
            {
                auto sqrtOp = [](ValueType x) { return std::sqrt(x); };
                output = ComputeOutput(sqrtOp);
            }
            break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void UnaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<UnaryOperationNode<ValueType>>(newInput, _operation);
        transformer.MapOutputPort(output, newNode->output);
    }

    // layers::CoordinateList MeanFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    // {
    //     // TODO: reimplement this using incremental computation (with an accumulator layer)
    //     auto it = featureOutputs.find(_inputFeatures[0]);
    //     if (it == featureOutputs.end())
    //     {
    //         throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Couldn't find input feature");
    //     }

    //     auto inputCoordinates = it->second;
    //     auto windowSize = GetWindowSize();

    //     // Compute mean
    //     auto bufferOutputCoordinates = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputCoordinates, windowSize);
    //     // TODO: find a better way to extract the per-channel coordinates
    //     std::vector<layers::CoordinateList> perChannelBufferOutputCoordinates;
    //     auto dimension = inputCoordinates.Size();
    //     auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());
    //     for(size_t channel = 0; channel < dimension; channel++)
    //     {
    //         perChannelBufferOutputCoordinates.push_back(shiftRegisterLayer.GetChannelOutputCoordinates(bufferOutputCoordinates, channel));
    //     }

    //     auto sumCoordinates = model.EmplaceLayer<layers::Sum>(perChannelBufferOutputCoordinates);
    //     auto divisorCoordinates = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>(dimension, windowSize));

    //     auto meanCoordinates = model.EmplaceLayer<layers::BinaryOperationLayer>(sumCoordinates, divisorCoordinates, layers::BinaryOperationLayer::OperationType::divide);
    //     return meanCoordinates;
    // }
}
