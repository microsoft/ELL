////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VarianceNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VarianceNode.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    template <typename ValueType>
    VarianceNode<ValueType>::VarianceNode(const model::OutputPortElementList<ValueType>& input, size_t windowSize) : Node({ &_input }, { &_output }), _input(this, input), _output(this, _input.Size()), _windowSize(windowSize)
    {
        auto dimension = input.Size();
        for (size_t index = 0; index < windowSize; ++index)
        {
            _samples.push_back(std::vector<ValueType>(dimension));
        }
        _runningSum = std::vector<ValueType>(dimension);
        _runningSquaredSum = std::vector<ValueType>(dimension);
    }

    template <typename ValueType>
    void VarianceNode<ValueType>::Compute() const
    {
        static auto squared = [](const ValueType& x) { return x * x; };

        auto inputSample = _input.GetValue();
        auto lastBufferedSample = _samples[0];
        _samples.push_back(inputSample);
        _samples.erase(_samples.begin());

        std::vector<ValueType> result(_input.Size());
        for (size_t index = 0; index < inputSample.size(); ++index)
        {
            _runningSum[index] += (inputSample[index] - lastBufferedSample[index]);
            _runningSquaredSum[index] += squared(inputSample[index]) - squared(lastBufferedSample[index]);
            result[index] = (_runningSquaredSum[index] - (squared(_runningSum[index]) / _windowSize)) / _windowSize;
        }
        _output.SetOutput(result);
    };

    template <typename ValueType>
    void VarianceNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<VarianceNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    template <typename ValueType>
    void VarianceNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformInputPort(_input);
        auto newNode = transformer.AddNode<VarianceNode<ValueType>>(newInput, _windowSize);
        transformer.MapOutputPort(output, newNode->output);
    }

    // layers::CoordinateList IncrementalVarianceFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    // {
    //     auto inputIterator = featureOutputs.find(_inputFeatures[0]);
    //     if (inputIterator == featureOutputs.end())
    //     {
    //         throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Couldn't find input feature");
    //     }
       
    //     auto inputCoordinates = inputIterator->second;
    //     auto inputDimension = inputCoordinates.Size();
    //     auto windowSize = GetWindowSize();

    //     // Make a layer holding the constant `windowSize`, broadcast to be wide enough to apply to all input dimensions
    //     auto divisor = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>{(double)windowSize});        
    //     auto divisorVector = layers::RepeatCoordinates(divisor, inputDimension);

    //     // Make a buffer that will hold `windowSize` samples
    //     auto bufferOutput = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputCoordinates, windowSize+1);
    //     auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());

    //     // Compute running sum by subtracting oldest value and adding newest
    //     auto oldestSample = shiftRegisterLayer.GetDelayedOutputCoordinates(bufferOutput, windowSize);
    //     auto diff = model.EmplaceLayer<layers::BinaryOperationLayer>(inputCoordinates, oldestSample, layers::BinaryOperationLayer::OperationType::subtract);
    //     auto runningSum = model.EmplaceLayer<layers::AccumulatorLayer>(diff);

    //     // Square the sum of inputs and divide by window size
    //     auto squaredSum = model.EmplaceLayer<layers::BinaryOperationLayer>(runningSum, runningSum, layers::BinaryOperationLayer::OperationType::multiply);
    //     auto normSquaredSum = model.EmplaceLayer<layers::BinaryOperationLayer>(squaredSum, divisorVector, layers::BinaryOperationLayer::OperationType::divide);
        
    //     // Accumulate running sum of squared samples 
    //     auto newValueSquared = model.EmplaceLayer<layers::BinaryOperationLayer>(inputCoordinates, inputCoordinates, layers::BinaryOperationLayer::OperationType::multiply);
    //     auto oldValueSquared = model.EmplaceLayer<layers::BinaryOperationLayer>(oldestSample, oldestSample, layers::BinaryOperationLayer::OperationType::multiply);
    //     auto diffSquared = model.EmplaceLayer<layers::BinaryOperationLayer>(newValueSquared, oldValueSquared, layers::BinaryOperationLayer::OperationType::subtract);
    //     auto runningSquaredSum = model.EmplaceLayer<layers::AccumulatorLayer>(diffSquared);

    //     // Compute variance from above values (var = (sum(x^2) - (sum(x)^2 / N)) / N )
    //     auto varianceTimesN = model.EmplaceLayer<layers::BinaryOperationLayer>(runningSquaredSum, normSquaredSum, layers::BinaryOperationLayer::OperationType::subtract);
    //     auto variance = model.EmplaceLayer<layers::BinaryOperationLayer>(varianceTimesN, divisorVector, layers::BinaryOperationLayer::OperationType::divide);
    //     return variance;
    // }
}
