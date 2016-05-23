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
#include "BinaryOpLayer.h"
#include "ShiftRegisterLayer.h"
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
    // IncrementalMeanFeature
    //

    IncrementalMeanFeature::IncrementalMeanFeature(Feature* inputFeature, size_t windowSize) : BufferedFeature({inputFeature}, windowSize) 
    {}

    std::vector<double> IncrementalMeanFeature::ComputeOutput() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& inputData = _inputFeatures[0]->GetOutput();
        auto rowSize = inputData.size();    
        if(rowSize == 0) // error, should we throw?
        {
            return inputData;
        }
        _outputDimension = rowSize;
        
        // get the oldest sample
        auto windowSize = GetWindowSize();
        auto oldData = GetDelayedSamples(windowSize-1);
        oldData.resize(rowSize);
        _runningSum.resize(rowSize);
         
        UpdateRowSamples(inputData);

        // update the running sum and output
        std::vector<double> result(rowSize);
        for(size_t index = 0; index < rowSize; ++index)
        {
            _runningSum[index] += (inputData[index] - oldData[index]);
            result[index] = _runningSum[index] / windowSize;
        }

        return result;
    }

    layers::CoordinateList IncrementalMeanFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        auto it = featureOutputs.find(_inputFeatures[0]);
        if (it == featureOutputs.end())
        {
            throw std::runtime_error("Couldn't find input feature");
        }
       
        auto inputCoordinates = it->second;
        auto dimension = inputCoordinates.Size();
        auto windowSize = GetWindowSize();

        // TODO: document
        auto bufferOutputCoordinates = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputCoordinates, windowSize+1);
        auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());
        auto oldestSample = shiftRegisterLayer.GetDelayedOutputCoordinates(bufferOutputCoordinates, windowSize);
        auto diff = model.EmplaceLayer<layers::BinaryOpLayer>(inputCoordinates, oldestSample, layers::BinaryOpLayer::OperationType::subtract);
        auto accumulator = model.EmplaceLayer<layers::AccumulatorLayer>(diff);
        auto divisor = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>(dimension, windowSize));        
        auto mean = model.EmplaceLayer<layers::BinaryOpLayer>(accumulator, divisor, layers::BinaryOpLayer::OperationType::divide);
        return mean;
    }

    std::unique_ptr<Feature> IncrementalMeanFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 4);
        Feature* inputFeature = previousFeatures[params[2]];
        uint64_t windowSize = ParseInt(params[3]);

        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return std::make_unique<IncrementalMeanFeature>(inputFeature, windowSize);
    }
}
