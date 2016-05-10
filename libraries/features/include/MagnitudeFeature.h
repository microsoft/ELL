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
    //
    // MagnitudeFeature
    //
    class MagnitudeFeature : public RegisteredFeature<MagnitudeFeature>
    {
    public:
        // Factory method
        static std::shared_ptr<MagnitudeFeature> Create(std::shared_ptr<Feature> inputFeature);
        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, Feature::FeatureMap& previousFeatures);

        explicit MagnitudeFeature(ctor_enable, const std::vector<std::shared_ptr<Feature>> &inputs);
        static constexpr const char* feature_name = "Magnitude";

    protected:
        virtual std::vector<double> ComputeOutput() const override;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const override;
    };
}
