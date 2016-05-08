////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IirFilterFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Feature.h"
#include "IirFilter.h"

#include <memory>
#include <vector>
#include <unordered_map>

namespace features
{
    //
    // IirFilterFeature
    //
    class IirFilterFeature : public UnaryFunctionFeature<IirFilterFeature>
    {
    public:
        static std::shared_ptr<IirFilterFeature> Create(std::shared_ptr<Feature> inputFeature, std::vector<double> a, std::vector<double> b);
        virtual size_t WarmupTime() const override;
        static constexpr const char* feature_name = "IirFilter";

        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures);

        explicit IirFilterFeature(ctor_enable, const std::vector<std::shared_ptr<Feature>>& inputs, std::vector<double> a, std::vector<double> b);

    protected:
        virtual std::vector<double> ComputeOutput() const override;
        virtual void AddDescription(std::vector<std::string>& description) const override;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const override;

    private:
        // TODO: make IirFilter work on vectors, so we can keep one filter instead of one per channel
        mutable std::vector<IirFilter> _filters;
        std::vector<double> _a;
        std::vector<double> _b;
    };
 }