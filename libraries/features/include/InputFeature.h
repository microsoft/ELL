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
    /// <summary> A feature that  represents an input to the featurizer. It's always the first feature to be added. </summary>
    class InputFeature : public RegisteredFeature<InputFeature>
    {
    public:
        InputFeature(ctor_enable, uint64_t size) : RegisteredFeature<InputFeature>({})
        {
            _outputDimension = size;
        };
                
        /// <summary> Sets the current value of the input </summary>
        void SetValue(std::vector<double> val);

        static constexpr const char* feature_name = "Input";

        static std::unique_ptr<Feature> Deserialize(std::vector<std::string> params, Feature::FeatureMap& prev_features);

    protected:
        virtual std::vector<double> ComputeOutput() const override;
        virtual void AddToDescription(std::vector<std::string>& description) const override;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const override;

        std::vector<double> _currentValue;
    };
}