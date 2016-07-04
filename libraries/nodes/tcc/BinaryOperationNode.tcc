////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryOperationNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryOperationNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cassert>

namespace nodes
{
    template <typename ValueType>
    BinaryOperationNode<ValueType>::BinaryOperationNode(const model::OutputPortElementList<ValueType>& input1, const model::OutputPortElementList<ValueType>& input2, OperationType operation) : Node({ &_input1, &_input2 }, { &_output }), _input1(this, input1), _input2(this, input2), _output(this, _input1.Size()), _operation(operation)
    {
        assert(input1.Size() == input2.Size());
    }

    template <typename ValueType>
    template <typename Operation>
    std::vector<ValueType> BinaryOperationNode<ValueType>::ComputeOutput(Operation&& fn) const
    {
        auto output = std::vector<ValueType>(_input1.Size());
        for (size_t index = 0; index < _input1.Size(); index++)
        {
            output[index] = fn(_input1[index], _input2[index]);
        }
        return output;
    }

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Compute() const
    {
        std::vector<ValueType> output;
        switch (_operation)
        {
            case OperationType::add:
                output = ComputeOutput([](ValueType x, ValueType y) { return x + y; });
                break;
            case OperationType::subtract:
                output = ComputeOutput([](ValueType x, ValueType y) { return x - y; });
                break;
            case OperationType::multiply:
                output = ComputeOutput([](ValueType x, ValueType y) { return x * y; });
                break;
            case OperationType::divide:
                output = ComputeOutput([](ValueType x, ValueType y) { return x / y; });
                break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "Unknown operation type");
        }
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void BinaryOperationNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput1 = transformer.TransformInputPort(_input1);
        auto newInput2 = transformer.TransformInputPort(_input2);
        auto newNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newInput1, newInput2, _operation);
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
