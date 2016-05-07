////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IirFilterFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Feature.h"
#include "UnaryFunctionFeature.h"

#include <memory>
#include <vector>

namespace features
{
    //
    // IirFilterFeature
    //
    class IirFilterFeature : public UnaryFunctionFeature<IirFilterFeature>
    {
    public:
        static std::shared_ptr<IirFilterFeature> Create(std::shared_ptr<Feature> inputFeature, std::vector<double> a, std::vector<double> b);
        virtual size_t WarmupTime() const;
        static constexpr const char* feature_name = "IirFilter";

        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures);
        explicit IirFilterFeature(ctor_enable, std::shared_ptr<Feature> inputFeature, std::vector<double> a, std::vector<double> b);

        virtual layers::CoordinateList AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;

    protected:
        virtual std::vector<double> ComputeOutput() const;
        virtual void AddDescription(std::vector<std::string>& description) const;

    private:
        // TODO: make IirFilter work on vectors, so we can keep one filter instead of one per channel
        mutable std::vector<IirFilter> _filters;
        std::vector<double> _a;
        std::vector<double> _b;
    };
 }