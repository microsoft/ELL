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

        virtual layers::CoordinateList AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const;
    protected:
        virtual std::vector<double> ComputeOutput() const;
    };
}

#include "../tcc/UnaryFunctionFeature.tcc"