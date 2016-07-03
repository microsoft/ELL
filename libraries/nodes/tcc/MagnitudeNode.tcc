////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MagnitudeNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MagnitudeNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>
#include <cmath>

namespace nodes
{
    template <typename ValueType>
    MagnitudeNode<ValueType>::MagnitudeNode(const model::OutputPortElementList<ValueType>& input) : Node({&_input}, {&_output}), _input(this, input), _output(this, 1)
    {
    }

    template <typename ValueType>
    void MagnitudeNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for(size_t index = 0; index < _input.Size(); ++index)
        {
            auto v = _input[index];
            result += (v*v);
        }
        _output.SetOutput({std::sqrt(result)});
    };

    template <typename ValueType>
    void MagnitudeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MagnitudeNode<ValueType>>(newInput);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MagnitudeNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MagnitudeNode<ValueType>>(newInput);
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
