////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MagnitudeFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Feature.h"

#include <string>
#include <vector>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <functional>

namespace features
{
    /// <summary> A feature that takes a vector input and returns its magnitude (L2 norm) </summary>
    class MagnitudeFeature : public Feature
    {
    public:
        MagnitudeFeature(Feature* inputFeature) : Feature({inputFeature}) {}
        static std::unique_ptr<Feature> Deserialize(std::vector<std::string> params, Feature::FeatureMap& previousFeatures);

        virtual std::string FeatureType() const override{ return feature_name; }
        static constexpr const char* feature_name = "Magnitude";

    protected:
        virtual std::vector<double> ComputeOutput() const override;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const override;
    };
}
