////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputFeature.h"
#include "StringUtil.h"

#include <cassert>

namespace features
{
    //
    // InputFeature
    //
    
    InputFeature::InputFeature(ctor_enable, uint64_t size) 
    {
        _numColumns = size;
    }

    std::shared_ptr<InputFeature> InputFeature::Create(uint64_t size)
    {
        return Allocate(size);
    }

    std::vector<double> InputFeature::ComputeValue() const
    {
        return _currentValue;
    }

    void InputFeature::SetValue(std::vector<double> val)
    {
        _currentValue = val;
        assert(_numColumns == val.size());
        SetDirty(true); // propagates through graph
    }

    std::vector<std::string> InputFeature::GetDescription() const
    {
        using std::to_string;
        std::vector<std::string> result = Feature::GetDescription();

        result.push_back(to_string(_numColumns));
        return result;
    }

    std::shared_ptr<Feature> InputFeature::Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures)
    {
        assert(params.size() == 1);
        uint64_t size = ParseInt(params[0]);
        return Create(size);
    }    
}