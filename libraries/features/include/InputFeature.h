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

namespace features
{
   //
    // InputFeature
    //
    class InputFeature : public RegisteredFeature<InputFeature>
    {
    public:
        static std::shared_ptr<InputFeature> Create(uint64_t size);
        void SetValue(std::vector<double> val);

        static constexpr const char* feature_name = "Input";

        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, FeatureMap& prev_features);
        explicit InputFeature(ctor_enable, uint64_t size);

        virtual layers::CoordinateList AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;

    protected:
        virtual std::vector<double> ComputeOutput() const;
        virtual void AddDescription(std::vector<std::string>& description) const;

        std::vector<double> _currentValue;
    };
}