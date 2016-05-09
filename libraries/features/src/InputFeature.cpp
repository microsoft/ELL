////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputFeature.h"
#include "StringUtil.h"

#include <stdexcept>
#include <cassert>

namespace features
{
    //
    // InputFeature
    //
    
    InputFeature::InputFeature(ctor_enable, const std::vector<std::shared_ptr<Feature>>& inputs, uint64_t size) : RegisteredFeature<InputFeature>(inputs)
    {
        _numColumns = size;
    }

    std::shared_ptr<InputFeature> InputFeature::Create(uint64_t size)
    {
        return Allocate({}, size);
    }

    std::vector<double> InputFeature::ComputeOutput() const
    {
        return _currentValue;
    }

    void InputFeature::SetValue(std::vector<double> val)
    {
        _currentValue = val;
        assert(_numColumns == val.size());
        SetDirtyFlag(true); // propagates through graph
    }

    layers::CoordinateList InputFeature::AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const
    {
        layers::CoordinateList inputCoordinates;
        
        throw std::runtime_error("Not implemented");
        return inputCoordinates;    
    }
    
    void InputFeature::AddDescription(std::vector<std::string>& description) const
    {
        using std::to_string;
        description.push_back(to_string(_numColumns));
    }

    std::shared_ptr<Feature> InputFeature::Deserialize(std::vector<std::string> params, Feature::FeatureMap& previousFeatures)
    {
        assert(params.size() == 3);
        uint64_t size = ParseInt(params[2]);
        return Create(size);
    }    
}