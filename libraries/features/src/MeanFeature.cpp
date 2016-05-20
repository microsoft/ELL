////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MeanFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MeanFeature.h"
#include "Feature.h"
// #include "VectorMath.h"
#include "StringUtil.h"

// layers
#include "UnaryOpLayer.h"
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
    double VecMean(const std::vector<double>& vec)
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
            // compute mean (TODO: incrementally)
            double mean = VecMean(GetAllSamples(columnIndex));
            result[columnIndex] = mean;
        }

        _outputDimension = rowSize;
        return result;
    }

    layers::CoordinateList MeanFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        auto it = featureOutputs.find(_inputFeatures[0]);
        if (it == featureOutputs.end())
        {
            throw std::runtime_error("Couldn't find input feature");
        }
       
        throw std::runtime_error("MeanFeature unimplemented");
        auto inputCoordinates = it->second;
        return inputCoordinates;
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
