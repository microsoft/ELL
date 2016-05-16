////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Feature.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace features
{    
    //
    // Feature implementation
    //
    template <typename FeatureT>
    void Feature::RegisterFeatureType()
    {
        RegisterDeserializeFunction(FeatureT::feature_name, FeatureT::Deserialize);
    }
}
