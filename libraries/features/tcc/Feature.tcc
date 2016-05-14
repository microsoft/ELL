////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Feature.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <unordered_set>

namespace features
{    
    //
    // RegisteredFeature implementation
    //
    template <typename FeatureT>
    void RegisteredFeature<FeatureT>::RegisterFeature()
    {
        RegisterDeserializeFunction(FeatureT::feature_name, FeatureT::Deserialize);
    }

    template <typename FeatureType>
    void Feature<FeatureType>::RegisterFeatureType()
    {        
        RegisterDeserializeFunction(FeatureT::feature_name, FeatureT::Deserialize);
    }

    template <typename FeatureT>
    RegisteredFeature<FeatureT>::RegisteredFeature(const std::vector<Feature*>& inputs) : Feature(inputs)
    {        
    }

    template <typename FeatureT>
    std::string RegisteredFeature<FeatureT>::FeatureType() const
    {
        return FeatureT::feature_name;
    }
}
