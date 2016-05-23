////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VarianceFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VarianceFeature.h"
#include "MeanFeature.h"
#include "Feature.h"
#include "StringUtil.h"

// layers
#include "ShiftRegisterLayer.h"
#include "ConstantLayer.h"
#include "BinaryOpLayer.h"
#include "Sum.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace
{
    double VectorMean(const std::vector<double>& vec)
    {
        if(vec.size() == 0) return 0.0;
        double sum = 0.0;
        for (auto x : vec)
        {
            sum += x;
        }
        return sum / vec.size();
    }

    double VectorVariance(const std::vector<double>& vec, double mean)
    {
        if(vec.size() == 0) return 0.0;
        double var = 0.0;
        for (auto x : vec)
        {
            double diff = x - mean;
            var += diff*diff;
        }
        return var / vec.size();
    }
}

namespace features
{
    //
    // VarianceFeature
    //

    VarianceFeature::VarianceFeature(Feature* inputFeature, size_t windowSize)  : BufferedFeature({inputFeature}, windowSize) 
    {
    }
    
    //VarianceFeature::VarianceFeature(Feature* inputFeature, Feature* meanFeature, size_t windowSize)  : BufferedFeature({inputFeature}, windowSize), _meanFeature(meanFeature)
    //{}

    std::vector<double> VarianceFeature::ComputeOutput() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& inputData = _inputFeatures[0]->GetOutput();
        if(inputData.size() == 0) // error, should we throw?
        {
            return inputData;
        }
        
        UpdateRowSamples(inputData);
        
        auto rowSize = inputData.size();    
        // average windows and put in result
        std::vector<double> result(rowSize);
        for (size_t columnIndex = 0; columnIndex < rowSize; columnIndex++)
        {
            // compute variance (TODO: incrementally)
            auto samples = GetAllSamples(columnIndex);
            double mean = VectorMean(samples);
            double variance = VectorVariance(samples, mean);
            result[columnIndex] = variance;
        }

        _outputDimension = rowSize;
        return result;
    }

    layers::CoordinateList VarianceFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        // TODO: reimplement this using incremental computation (with an accumulator layer)
        auto it = featureOutputs.find(_inputFeatures[0]);
        if (it == featureOutputs.end())
        {
            throw std::runtime_error("Couldn't find input feature");
        }
       
        auto inputCoordinates = it->second;
        auto dimension = inputCoordinates.Size();
        auto windowSize = GetWindowSize();

        // -- Compute mean
        auto bufferOutputCoordinates = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputCoordinates, windowSize);        

        // TODO: find a better way to extract the per-channel coordinates
        std::vector<layers::CoordinateList> perChannelBufferOutputCoordinates;
        auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());
        for(size_t channel = 0; channel < dimension; ++channel)
        {
            perChannelBufferOutputCoordinates.push_back(shiftRegisterLayer.GetChannelOutputCoordinates(bufferOutputCoordinates, channel));
        }
        
        

        auto sumCoordinates = model.EmplaceLayer<layers::Sum>(perChannelBufferOutputCoordinates);
        auto divisorCoordinates = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>(dimension, windowSize));
        
        auto meanCoordinates = model.EmplaceLayer<layers::BinaryOpLayer>(sumCoordinates, divisorCoordinates, layers::BinaryOpLayer::OperationType::divide);
        // -- Now compute variance

        // Expand meanOutputCoordinates to be the same dimension as buffered input (by concatenating together _W_ copies)
        layers::CoordinateList expandedMeanCoordinates;
        for(size_t index = 0; index < windowSize; ++index)
        {
            for(size_t coordIndex = 0; coordIndex < dimension; ++coordIndex)
            {
                expandedMeanCoordinates.AddCoordinate(meanCoordinates[coordIndex]);
            }
        }
        
        // Subtract mean from windowed input
        auto subtractMeanCoordinates = model.EmplaceLayer<layers::BinaryOpLayer>(expandedMeanCoordinates, bufferOutputCoordinates, layers::BinaryOpLayer::OperationType::subtract);

        // Square the difference
        auto squaredDiffCoordinates = model.EmplaceLayer<layers::BinaryOpLayer>(subtractMeanCoordinates, subtractMeanCoordinates, layers::BinaryOpLayer::OperationType::multiply);

        std::vector<layers::CoordinateList> perChannelSquaredDiffCoordinates;
        for(size_t channel = 0; channel < dimension; ++channel)
        {
            layers::CoordinateList channelCoordinates;
            for(size_t index = 0; index < windowSize; ++index)
            {
                channelCoordinates.AddCoordinate(squaredDiffCoordinates[index*dimension+channel]);
            }
            perChannelSquaredDiffCoordinates.push_back(channelCoordinates);
        }

        // Sum of (x-mean)^2
        auto sumDifferenceCoordinates = model.EmplaceLayer<layers::Sum>(perChannelSquaredDiffCoordinates);
        
        // Divide by N, and we're done
        auto varianceCoordinates = model.EmplaceLayer<layers::BinaryOpLayer>(sumDifferenceCoordinates, divisorCoordinates, layers::BinaryOpLayer::OperationType::divide);
        return varianceCoordinates;
    }

    std::unique_ptr<Feature> VarianceFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 4);
        Feature* inputFeature = previousFeatures[params[2]];
        uint64_t windowSize = ParseInt(params[3]);

        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return std::make_unique<VarianceFeature>(inputFeature, windowSize);
    }
}
