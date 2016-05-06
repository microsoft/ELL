////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryFunctionFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Feature.h"
#include "IirFilter.h"

#include <string>
#include <vector>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <functional>

namespace features
{
    //
    // UnaryFunctionFeature
    //
    template <typename FeatureT>
    class UnaryFunctionFeature : public RegisteredFeature<FeatureT>
    {
    public:
        UnaryFunctionFeature(std::shared_ptr<Feature> inputFeature);

    protected:
        using RegisteredFeature<FeatureT>::AddInputFeature;

        // Allocates a new shared_ptr to a feature of type FeatureType
        template <typename ... Args>
        static std::shared_ptr<FeatureT> Allocate(std::shared_ptr<Feature>, Args... args);

    private:
    };

    //
    // MagnitudeFeature
    //
    class MagnitudeFeature : public UnaryFunctionFeature<MagnitudeFeature>
    {
    public:
        static std::shared_ptr<MagnitudeFeature> Create(std::shared_ptr<Feature> inputFeature);
        static constexpr const char* feature_name = "Magnitude";

        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures);
        explicit MagnitudeFeature(ctor_enable, std::shared_ptr<Feature> inputFeature);

    protected:
        virtual std::vector<double> ComputeValue() const;
    };


    //
    // IirFilterFeature
    //
    class IirFilterFeature : public UnaryFunctionFeature<IirFilterFeature>
    {
    public:
        static std::shared_ptr<IirFilterFeature> Create(std::shared_ptr<Feature> inputFeature, std::vector<double> a, std::vector<double> b);
        virtual size_t WarmupTime() const;
        static constexpr const char* feature_name = "IirFilter";

        virtual std::vector<std::string> GetDescription() const;

        static std::shared_ptr<Feature> Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures);
        explicit IirFilterFeature(ctor_enable, std::shared_ptr<Feature> inputFeature, std::vector<double> a, std::vector<double> b);

    protected:
        virtual std::vector<double> ComputeValue() const;

    private:
        // TODO: make IirFilter work on vectors, so we can keep one filter instead of one per channel
        mutable std::vector<IirFilter> _filters;
        std::vector<double> _a;
        std::vector<double> _b;
    };
}

#include "../tcc/UnaryFunctionFeature.tcc"