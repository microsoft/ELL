////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IncrementalVarianceFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IncrementalVarianceFeature.h"
#include "MeanFeature.h"
#include "Feature.h"
#include "StringUtil.h"

// layers
#include "ShiftRegisterLayer.h"
#include "ConstantLayer.h"
#include "AccumulatorLayer.h"
#include "BinaryOpLayer.h"
#include "Sum.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace features
{
    //
    // IncrementalVarianceFeature
    //

    IncrementalVarianceFeature::IncrementalVarianceFeature(Feature* inputFeature, size_t windowSize)  : BufferedFeature({inputFeature}, windowSize) 
    {
    }
    
    std::vector<double> IncrementalVarianceFeature::ComputeOutput() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& inputData = _inputFeatures[0]->GetOutput();
        auto rowSize = inputData.size();    
        if(rowSize == 0) // error, should we throw?
        {
            return inputData;
        }
        _outputDimension = rowSize;
        auto windowSize = GetWindowSize();
        
        // get the oldest sample
        auto oldData = GetDelayedSamples(windowSize-1);
        oldData.resize(rowSize);
        _runningSum.resize(rowSize);
        _runningSumSq.resize(rowSize);
         
        UpdateRowSamples(inputData);

        // update the running sum and output
        std::vector<double> result(rowSize);
        for(size_t index = 0; index < rowSize; ++index)
        {
            auto newVal = inputData[index];
            auto oldVal = oldData[index];
            _runningSum[index] += (newVal - oldVal);
            _runningSumSq[index] += (newVal*newVal - oldVal*oldVal);

            auto variance = (_runningSumSq[index] - (_runningSum[index]*_runningSum[index] / windowSize)) / windowSize;
            result[index] = variance;
        }

        return result;
    }

    layers::CoordinateList IncrementalVarianceFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        auto it = featureOutputs.find(_inputFeatures[0]);
        if (it == featureOutputs.end())
        {
            throw std::runtime_error("Couldn't find input feature");
        }
       
        auto inputCoordinates = it->second;
        auto dimension = inputCoordinates.Size();
        auto windowSize = GetWindowSize();

        // TODO: document!
        auto divisor = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>(dimension, windowSize)); // for dividing by window size

        auto bufferOutput = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputCoordinates, windowSize+1);
        auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());
        auto oldestSample = shiftRegisterLayer.GetDelayedOutputCoordinates(bufferOutput, windowSize);
        auto diff = model.EmplaceLayer<layers::BinaryOpLayer>(inputCoordinates, oldestSample, layers::BinaryOpLayer::OperationType::subtract);
        auto accumValue = model.EmplaceLayer<layers::AccumulatorLayer>(diff);
        auto squaredSum = model.EmplaceLayer<layers::BinaryOpLayer>(accumValue, accumValue, layers::BinaryOpLayer::OperationType::multiply);
        auto normsquaredSum = model.EmplaceLayer<layers::BinaryOpLayer>(squaredSum, divisor, layers::BinaryOpLayer::OperationType::divide);
        
        // accumulate squared samples
        auto newValueSquared = model.EmplaceLayer<layers::BinaryOpLayer>(inputCoordinates, inputCoordinates, layers::BinaryOpLayer::OperationType::multiply);
        auto oldValueSquared = model.EmplaceLayer<layers::BinaryOpLayer>(oldestSample, oldestSample, layers::BinaryOpLayer::OperationType::multiply);
        auto diffSquared = model.EmplaceLayer<layers::BinaryOpLayer>(newValueSquared, oldValueSquared, layers::BinaryOpLayer::OperationType::subtract);
        auto accumSquared = model.EmplaceLayer<layers::AccumulatorLayer>(diffSquared);

        // compute variance from above
        auto varianceTimesN = model.EmplaceLayer<layers::BinaryOpLayer>(accumSquared, normsquaredSum, layers::BinaryOpLayer::OperationType::subtract);
        auto variance = model.EmplaceLayer<layers::BinaryOpLayer>(varianceTimesN, divisor, layers::BinaryOpLayer::OperationType::divide);
        return variance;
    }

    std::unique_ptr<Feature> IncrementalVarianceFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 4);
        Feature* inputFeature = previousFeatures[params[2]];
        uint64_t windowSize = ParseInt(params[3]);

        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return std::make_unique<IncrementalVarianceFeature>(inputFeature, windowSize);
    }
}
