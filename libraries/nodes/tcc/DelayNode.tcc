////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DelayNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DelayNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    template <typename ValueType>
    DelayNode<ValueType>::DelayNode(const model::OutputPortElementList<ValueType>& input, size_t windowSize) : Node({&_input}, {&_output}), _input(this, input), _output(this, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for(size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Compute() const
    {
        auto lastBufferedSample = _samples[0];        
        _samples.push_back(_input.GetValue());
        _samples.erase(_samples.begin());
        _output.SetOutput(lastBufferedSample);
    };

    template <typename ValueType>
    void DelayNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void DelayNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<DelayNode<ValueType>>(newInput, _windowSize);
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
