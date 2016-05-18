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
        /// <summary> Constructor from magnitude feature </summary>
        /// <param name="inputFeature"> The feature to take the magnitude of </param>
        MagnitudeFeature(Feature* inputFeature) : Feature({inputFeature}) {}

        /// <summary> Creates a MagnitudeFeature from a vector of strings </summary>
        ///
        /// <param name="params"> The strings describing the parameters </params> 
        /// <param name="previousFeatures"> The map of existing features available as inputs. </params> 
        static std::unique_ptr<Feature> Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures);

        virtual std::string FeatureType() const override{ return feature_name; }
        static constexpr const char* feature_name = "Magnitude";

    protected:
        virtual std::vector<double> ComputeOutput() const override;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const override;
    };
}
