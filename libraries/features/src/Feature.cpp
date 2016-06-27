////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Feature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Feature.h"
#include "InputFeature.h"
#include "StringUtil.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace features
{
    //
    // feature base class
    //
    int Feature::_instanceCount = 0;
    std::unordered_map<std::string, Feature::CreateFunction> Feature::_createTypeMap;

    Feature::Feature(const std::vector<Feature*>& inputs) : _isDirty(true), _inputFeatures(inputs)
    {
        using std::to_string;
        // create id
        _instanceId = _instanceCount;
        ++_instanceCount;
        _id = "f_" + to_string(_instanceId);
        
        for(auto input: inputs)
        {
            input->AddDependent(this);
        }
    }

    Feature::Feature(std::string id, const std::vector<Feature*>& inputs) : _id(id), _isDirty(true), _inputFeatures(inputs) 
    {
        // TODO: ensure id is unique --- how?
        _instanceId = _instanceCount;
        ++_instanceCount;
        for(auto input: inputs)
        {
            input->AddDependent(this);
        }
    }

    std::vector<double> Feature::GetOutput() const
    {
        if (IsDirty() || _cachedValue.size() == 0)
        {
            _cachedValue = ComputeOutput();
            _isDirty = false; // Note: we don't call SetDirtyFlag(false) here, as that will start a cascade of SetDirtyFlag calls
        }

        return _cachedValue;
    }

    void Feature::Reset()
    {
        SetDirtyFlag(true);
        for (auto& f : _dependents)
        {
            f->Reset();
        }
    }

    size_t Feature::GetWarmupTime() const
    {
        size_t maxTime = 0;
        for (const auto& inputFeature : _inputFeatures)
        {
            maxTime = std::max(maxTime, inputFeature->GetWarmupTime());
        }
        return maxTime;
    }

    std::vector<std::string> Feature::GetDescription() const
    {
        std::vector<std::string> result;
        result.reserve(_inputFeatures.size() + 2);

        // Write out our id and type
        result.push_back(Id());
        result.push_back(FeatureType());

        // Write out ids of everybody I depend on
        for (const auto& inputFeature : _inputFeatures)
        {
            result.push_back(inputFeature->Id());
        }
        
        // Now add subclass-specific parts
        AddToDescription(result);
        return result;
    }

    std::vector<std::string> Feature::GetColumnDescriptions() const
    {
        using std::to_string;
        std::vector<std::string> result;
        auto size = GetOutputDimension();
        for (int index = 0; index < size; index++)
        {
            result.push_back(FeatureType() + "_" + to_string(index));
        }
        return result;
    }

    const std::vector<Feature*>& Feature::GetInputFeatures() const
    {
        return _inputFeatures;
    }

    std::vector<std::string> Feature::GetRegisteredTypes()
    {
        std::vector<std::string> result;
        for (const auto& entry : _createTypeMap)
        {
            result.push_back(entry.first);
        }
        return result;
    }

    void Feature::RegisterCreateFunction(std::string class_name, CreateFunction create_fn)
    {
        _createTypeMap[class_name] = create_fn;
    }

    // Adds a feature to our list of dependent features.
    // We need to keep track of dependents for two reasons:
    //   1) traversing the whole graph (not just the active graph)
    //   2) propagating the 'dirty' flag when new input arrives  
    void Feature::AddDependent(Feature* f)
    {
        _dependents.push_back(f);
    }

    void Feature::SetDirtyFlag(bool dirty) const
    {
        _isDirty = dirty;
        if (dirty) // Propagate dirtyness
        {
            for (auto& f : _dependents)
            {
                assert(f != nullptr);
                f->SetDirtyFlag(true);
            }
        }
    }
    
    std::unique_ptr<Feature> Feature::FromDescription(const std::vector<std::string>& description, Feature::FeatureMap& previousFeatures)
    {
        std::string featureId = TrimString(description[0]);
        std::string featureClass = TrimString(description[1]);

        if(previousFeatures.find(featureId) != previousFeatures.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "Error deserializing feature description: non-unique ID " + featureId);
        }
        
        auto createFunction = _createTypeMap[featureClass];
        if (createFunction == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "Error deserializing feature description: unknown feature type '" + featureClass + "'");
        }
        return createFunction(description, previousFeatures);
    }
}
