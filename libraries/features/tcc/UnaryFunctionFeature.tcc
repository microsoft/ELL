////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryFunctionFeature.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>

namespace features
{
    template <typename FeatureT>
    UnaryFunctionFeature<FeatureT>::UnaryFunctionFeature(std::shared_ptr<Feature> inputFeature)
    {
        AddInputFeature(inputFeature);
    }

    template <typename FeatureT>
    template <typename... Args>
    std::shared_ptr<FeatureT> UnaryFunctionFeature<FeatureT>::Allocate(std::shared_ptr<Feature> inputFeature, Args ...args)
    {
        auto ptr = RegisteredFeature<FeatureT>::Allocate(inputFeature, args...);
        inputFeature->AddDependent(ptr);
        return ptr;
    }
}
