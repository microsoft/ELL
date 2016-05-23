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
        // TODO: change feature_name to static GetTypeName() and call that there
        RegisterCreateFunction(FeatureT::feature_name, FeatureT::Create);
    }
}
