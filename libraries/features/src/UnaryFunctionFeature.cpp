////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryFunctionFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryFunctionFeature.h"
#include "Feature.h"
#include "VectorMath.h"
#include "StringUtil.h"
#include "UnaryOpLayer.h"
#include "BinaryOpLayer.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace features
{
    //
    // MagnitudeFeature
    //
    std::shared_ptr<MagnitudeFeature> MagnitudeFeature::Create(std::shared_ptr<Feature> inputFeature)
    {
        return Allocate(inputFeature);
    }

    MagnitudeFeature::MagnitudeFeature(ctor_enable, std::shared_ptr<Feature> inputFeature) : UnaryFunctionFeature<MagnitudeFeature>(inputFeature)
    {}

    std::vector<double> MagnitudeFeature::ComputeOutput() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& in_vec = _inputFeatures[0]->GetOutput();
        double m = 0;
        for (auto v : in_vec)
        {
            m += v*v;
        }
        std::vector<double> result = { std::sqrt(m) };
        _numColumns = result.size();
        return result;
    }

    layers::CoordinateList MagnitudeFeature::AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const
    {
        auto multLayer = std::make_unique<layers::BinaryOpLayer>(inputCoordinates, inputCoordinates, layers::BinaryOpLayer::OperationType::multiply);
        auto squaredOutputCoordinates = model.AddLayer(std::move(multLayer));
        auto sqrtLayer = std::make_unique<layers::UnaryOpLayer>(squaredOutputCoordinates, layers::UnaryOpLayer::OperationType::sqrt);
        auto outputCoordinates = model.AddLayer(std::move(sqrtLayer)); 
        return outputCoordinates;
    }

    std::shared_ptr<Feature> MagnitudeFeature::Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures)
    {
        std::shared_ptr<Feature> inputFeature = previousFeatures[params[2]];
        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return Create(inputFeature);
    }
}