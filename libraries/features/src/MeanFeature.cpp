////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MeanFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
}

namespace features
{
    //
    // MeanFeature
    //

    std::vector<double> MeanFeature::ComputeOutput() const
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
            auto samples = GetAllSamples(columnIndex);
            double mean = VectorMean(samples);
            result[columnIndex] = mean;
        }

        _outputDimension = rowSize;
        return result;
    }

    layers::CoordinateList MeanFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        // TODO: reimplement this using incremental computation (with an accumulator layer)
        auto it = featureOutputs.find(_inputFeatures[0]);
        if (it == featureOutputs.end())
        {
            throw std::runtime_error("Couldn't find input feature");
        }
       
        auto inputCoordinates = it->second;
        auto windowSize = GetWindowSize();

        // Compute mean
        auto bufferOutputCoordinates = model.EmplaceLayer<layers::ShiftRegisterLayer>(inputCoordinates, windowSize);        
        // TODO: find a better way to extract the per-channel coordinates
        std::vector<layers::CoordinateList> perChannelBufferOutputCoordinates;
        auto dimension = inputCoordinates.Size();
        auto shiftRegisterLayer = dynamic_cast<const layers::ShiftRegisterLayer&>(model.GetLastLayer());
        for(size_t channel = 0; channel < dimension; channel++)
        {
            perChannelBufferOutputCoordinates.push_back(shiftRegisterLayer.GetChannelOutputCoordinates(bufferOutputCoordinates, channel));
        }

        auto sumCoordinates = model.EmplaceLayer<layers::Sum>(perChannelBufferOutputCoordinates);
        auto divisorCoordinates = model.EmplaceLayer<layers::ConstantLayer>(std::vector<double>(dimension, windowSize));
        
        auto meanCoordinates = model.EmplaceLayer<layers::BinaryOpLayer>(sumCoordinates, divisorCoordinates, layers::BinaryOpLayer::OperationType::divide);
        return meanCoordinates;
    }

    std::unique_ptr<Feature> MeanFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 4);
        Feature* inputFeature = previousFeatures[params[2]];
        uint64_t windowSize = ParseInt(params[3]);

        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return std::make_unique<MeanFeature>(inputFeature, windowSize);
    }
}
