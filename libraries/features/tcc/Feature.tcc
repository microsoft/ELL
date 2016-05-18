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
        RegisterCreateFunction(FeatureT::feature_name, FeatureT::Create);
    }
}
