////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MagnitudeFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MagnitudeFeature.h"
#include "Feature.h"
// #include "VectorMath.h"
#include "StringUtil.h"

// layers
#include "UnaryOperationLayer.h"
#include "BinaryOperationLayer.h"
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
    // MagnitudeFeature
    //

    std::vector<double> MagnitudeFeature::ComputeOutput() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& inputData = _inputFeatures[0]->GetOutput();
        double m = 0;
        for (auto v : inputData)
        {
            m += v*v;
        }
        std::vector<double> result = { std::sqrt(m) };
        _outputDimension = 1;
        return result;
    }

    layers::CoordinateList MagnitudeFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        auto it = featureOutputs.find(_inputFeatures[0]);
        if (it == featureOutputs.end())
        {
            throw std::runtime_error("Couldn't find input feature");
        }
       
        auto inputCoordinates = it->second;
        auto squaredOutputCoordinates = model.EmplaceLayer<layers::BinaryOperationLayer>(inputCoordinates, inputCoordinates, layers::BinaryOperationLayer::OperationType::multiply); 

        auto sumOutputCoordinates = model.EmplaceLayer<layers::Sum>(squaredOutputCoordinates);

        auto outputCoordinates = model.EmplaceLayer<layers::UnaryOperationLayer>(sumOutputCoordinates, layers::UnaryOperationLayer::OperationType::sqrt); 
        return outputCoordinates;
    }

    std::unique_ptr<Feature> MagnitudeFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        Feature* inputFeature = previousFeatures[params[2]];
        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return std::make_unique<MagnitudeFeature>(inputFeature);
    }
}