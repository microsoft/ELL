////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Feature.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace features
{
   //
    // InputFeature -- Represents an input to the FeatureSet
    //
    class InputFeature : public RegisteredFeature<InputFeature>
    {
    public:
        static std::shared_ptr<InputFeature> Create(uint64_t size);
        void SetValue(std::vector<double> val);

        static constexpr const char* feature_name = "Input";

        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, Feature::FeatureMap& prev_features);
        explicit InputFeature(ctor_enable, const std::vector<std::shared_ptr<Feature>>&, uint64_t size);

    protected:
        virtual std::vector<double> ComputeOutput() const;
        virtual void AddDescription(std::vector<std::string>& description) const;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const;

        std::vector<double> _currentValue;
    };
}