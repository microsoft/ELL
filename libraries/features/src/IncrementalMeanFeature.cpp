////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IncrementalMeanFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IncrementalMeanFeature.h"
#include "Feature.h"
#include "StringUtil.h"

// layers
#include "AccumulatorLayer.h"
#include "ConstantLayer.h"
#include "BinaryOperationLayer.h"
#include "ShiftRegisterLayer.h"
#include "Sum.h"
#include "CoordinateListTools.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include <unordered_map>

namespace features
{
    //
    // IncrementalMeanFeature
    //

    IncrementalMeanFeature::IncrementalMeanFeature(Feature* inputFeature, size_t windowSize) : BufferedFeature({inputFeature}, windowSize) 
    {}

    IncrementalMeanFeature::IncrementalMeanFeature(const std::string& id, Feature* inputFeature, size_t windowSize) : BufferedFeature(id, {inputFeature}, windowSize) 
    {}

    std::vector<double> IncrementalMeanFeature::ComputeOutput() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& inputData = _inputFeatures[0]->GetOutput();
        auto inputDimension = inputData.size();    
        if(inputDimension == 0) 
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Invalid input of size zero");
            return inputData;
        }
        _outputDimension = inputDimension;
        auto windowSize = GetWindowSize();
        
        // get the oldest sample
        auto oldData = GetDelayedSamples(windowSize-1);
        oldData.resize(inputDimension);
        _runningSum.resize(inputDimension);
         
        UpdateRowSamples(inputData);

        // update the running sum and output
        std::vector<double> result(inputDimension);
        for(size_t index = 0; index < inputDimension; ++index)
        {
            _runningSum[index] += (inputData[index] - oldData[index]);
            result[index] = _runningSum[index] / windowSize;
        }

        return result;
    }

    layers::CoordinateList IncrementalMeanFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        auto inputIterator = featureOutputs.find(_inputFeatures[0]);
        if (inputIterator == featureOutputs.end())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Couldn't find input feature");
        }

        auto inputData = inputIterator->second;
        auto inputDimension = inputData.Size();
        auto windowSize = GetWindowSize();

        // We implement mean by keeping a running sum over `windowSize` samples, and then divide 
        // the result by the number of samples
        
        // Make a buffer that will hold `windowSize` samples
        auto bufferOutput = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputData, windowSize+1);
        auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());

        // Compute running sum by subtracting oldest value and adding newest
        auto oldestSample = shiftRegisterLayer.GetDelayedOutputCoordinates(bufferOutput, windowSize);
        auto diff = model.EmplaceLayer<layers::BinaryOperationLayer>(inputData, oldestSample, layers::BinaryOperationLayer::OperationType::subtract);
        auto runningSum = model.EmplaceLayer<layers::AccumulatorLayer>(diff);

        // Make a layer holding the constant `windowSize`, broadcast to be wide enough to apply to all input dimensions, and divide running sum by it
        auto divisor = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>{(double)windowSize});
        auto divisorVector = layers::RepeatCoordinates(divisor, inputDimension);
        auto mean = model.EmplaceLayer<layers::BinaryOperationLayer>(runningSum, divisorVector, layers::BinaryOperationLayer::OperationType::divide);
        return mean;
    }

    std::unique_ptr<Feature> IncrementalMeanFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 4);
        auto featureId = params[0];
        Feature* inputFeature = previousFeatures[params[2]];
        uint64_t windowSize = ParseInt(params[3]);

        if (inputFeature == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "Error deserializing feature description: unknown input feature " + params[2]);
        }
        return std::make_unique<IncrementalMeanFeature>(featureId, inputFeature, windowSize);
    }
}
