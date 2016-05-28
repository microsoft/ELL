////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Feature.h"
#include "InputFeature.h"
#include "StringUtil.h"

#include <vector>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace features
{
    //
    // InputFeature
    //

    void InputFeature::SetValue(std::vector<double> val)
    {
        _currentValue = val;
        if(_outputDimension != val.size())
        {
            throw std::runtime_error("Input feature dimension doesn't match data size");
        }
        assert(_outputDimension == val.size());
        SetDirtyFlag(true); // propagates through graph
    }

    layers::CoordinateList InputFeature::AddToModel(layers::Model& model, const std::unordered_map<const Feature*, layers::CoordinateList>& featureOutputs) const
    {
        throw std::runtime_error("Not implemented");
    }
    
    void InputFeature::AddToDescription(std::vector<std::string>& description) const
    {
        using std::to_string;
        description.push_back(to_string(_outputDimension));
    }

    std::unique_ptr<Feature> InputFeature::Create(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 3);
        uint64_t size = ParseInt(params[2]);
        return std::make_unique<InputFeature>(size);
    }    
}