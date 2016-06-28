////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IncrementalVarianceFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Feature.h"
#include "BufferedFeature.h"
#include "CoordinateList.h"
#include "Model.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace features
{
    /// <summary> 
    /// A feature that takes a vector input and returns its variance over some window of time. 
    /// This feature is computed incrementally, so it takes \f$O(1)\f$ time instead of \f$O(n)\f$ time.
    /// </summary>
    /// <remarks> Computes variance by keeping a running sum of \f$x\f$ and \f$x^2\f$, and computes \f$var = \frac{(\sum{x^2} - \frac{\sum{x}^2}{N})}{N}\f$ </remarks>
   class IncrementalVarianceFeature : public BufferedFeature
    {
    public:
        /// <summary> Constructor </summary>
        /// <param name="inputFeature"> The feature to take the variance of </param>
        /// <param name="windowSize"> The number of samples of history to use in computing the variance </param>
        IncrementalVarianceFeature(Feature* inputFeature, size_t windowSize);

        /// <summary> Constructor </summary>
        /// <param name="id"> The id to use for this feature </param>
        /// <param name="inputFeature"> The feature to take the variance of </param>
        /// <param name="windowSize"> The number of samples of history to use in computing the variance </param>
        IncrementalVarianceFeature(const std::string& id, Feature* inputFeature, size_t windowSize);

        /// <summary> Creates an `IncrementalVarianceFeature` from a vector of strings </summary>
        ///
        /// <param name="params"> The strings describing the parameters </paras> 
        /// <param name="previousFeatures"> The map of existing features available as inputs. </param> 
        static std::unique_ptr<Feature> Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures);

        virtual std::string FeatureType() const override { return feature_name; }
        static constexpr const char* feature_name = "IncrementalVariance";

    protected:
        virtual std::vector<double> ComputeOutput() const override;
        virtual layers::CoordinateList AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const override;

    private:
        mutable std::vector<double> _runningSum;
        mutable std::vector<double> _runningSumSq;
    };
}
