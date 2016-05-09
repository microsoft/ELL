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

    template <typename FeatureT>
    RegisteredFeature<FeatureT>::RegisteredFeature(const std::vector<std::shared_ptr<Feature>>& inputs) : Feature(inputs)
    {        
    }

    template <typename FeatureT>
    std::string RegisteredFeature<FeatureT>::FeatureType() const
    {
        return FeatureT::feature_name;
    }
    
    template <typename FeatureT>
    template <typename... Args>
    std::shared_ptr<FeatureT> RegisteredFeature<FeatureT>::Allocate(const std::vector<std::shared_ptr<Feature>>& inputs, Args ...args)
    {
        auto feature = std::make_shared<FeatureT>(ctor_enable(), inputs, args...);
        for(auto& input: inputs)
        {        
            input->AddDependent(feature);
        }
        return feature;
    }    
}
