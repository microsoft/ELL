////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MeanNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MeanNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    template <typename ValueType>
    MeanNode<ValueType>::MeanNode(const model::OutputPortElementList<ValueType>& input, size_t windowSize) : Node({&_input}, {&_output}), _input(this, input), _output(this, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for(size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void MeanNode<ValueType>::Compute() const
    {
        auto inputSample = _input.GetValue();
        auto lastBufferedSample = _samples[0];
        _samples.push_back(inputSample);
        _samples.erase(_samples.begin());

        std::vector<ValueType> result(_input.Size());
        for(size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index]-lastBufferedSample[index]);
            result[index] = _runningSum[index] / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void MeanNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MeanNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void MeanNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<MeanNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }
    
    // layers::CoordinateList IncrementalMeanFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    // {
    //     auto inputIterator = featureOutputs.find(_inputFeatures[0]);
    //     if (inputIterator == featureOutputs.end())
    //     {
    //         throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Couldn't find input feature");
    //     }

    //     auto inputData = inputIterator->second;
    //     auto inputDimension = inputData.Size();
    //     auto windowSize = GetWindowSize();

    //     // We implement mean by keeping a running sum over `windowSize` samples, and then divide 
    //     // the result by the number of samples
        
    //     // Make a buffer that will hold `windowSize` samples
    //     auto bufferOutput = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputData, windowSize+1);
    //     auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());

    //     // Compute running sum by subtracting oldest value and adding newest
    //     auto oldestSample = shiftRegisterLayer.GetDelayedOutputCoordinates(bufferOutput, windowSize);
    //     auto diff = model.EmplaceLayer<layers::BinaryOperationLayer>(inputData, oldestSample, layers::BinaryOperationLayer::OperationType::subtract);
    //     auto runningSum = model.EmplaceLayer<layers::AccumulatorLayer>(diff);

    //     // Make a layer holding the constant `windowSize`, broadcast to be wide enough to apply to all input dimensions, and divide running sum by it
    //     auto divisor = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>{(double)windowSize});
    //     auto divisorVector = layers::RepeatCoordinates(divisor, inputDimension);
    //     auto mean = model.EmplaceLayer<layers::BinaryOperationLayer>(runningSum, divisorVector, layers::BinaryOperationLayer::OperationType::divide);
    //     return mean;
    // }
}
