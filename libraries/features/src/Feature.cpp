////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Feature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Feature.h"
#include "InputFeature.h"
//#include "Dataset.h"
//#include "VectorMath.h"
#include "StringUtil.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace features
{
    //
    // feature base class
    //
    int Feature::_instanceCount = 0;
    std::unordered_map<std::string, std::function<std::shared_ptr<Feature>(std::vector<std::string>, FeatureMap&)>> Feature::_createTypeMap;

    Feature::Feature() : _isDirty(true)
    {
        using std::to_string;
        // create id
        _instanceId = _instanceCount;
        _instanceCount++;
        _id = "f_" + to_string(_instanceId);
    }

    Feature::Feature(std::string id) : _isDirty(true), _id(id)
    {
        // Need to make sure name is unique. Punt for now --- it's an error if you don't ((de)serialization will fail)
        Feature::_instanceCount++; // necessary?
    }

    Feature::~Feature()
    {
    }

    std::vector<double> Feature::Eval() const
    {
        if (IsDirty() || _cachedValue.size() == 0)
        {
            _cachedValue = ComputeValue();
            _isDirty = false; // Note: don't call SetDirty(false) here, as that will start a cascade of SetDirty calls
        }

        return _cachedValue;
    }

    std::string Feature::Id() const
    {
        return _id;
    }

    void Feature::AddDependent(std::shared_ptr<Feature> f)
    {
        _dependents.push_back(f);
    }

    layers::CoordinateList Feature::AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const
    {
        throw std::runtime_error("Not implemented");
        return inputCoordinates;
    }

    size_t Feature::NumColumns() const
    {
        return _numColumns;
    }

    bool Feature::HasOutput() const
    {
        return !IsDirty();
    }

    void Feature::SetDirty(bool dirty) const
    {
        _isDirty = dirty;
        if (dirty)
        {
            for (auto& f : _dependents)
            {
                assert(f != nullptr);
                f->SetDirty(true);
            }
        }
    }

    bool Feature::IsDirty() const
    {
        return _isDirty;
    }

    void Feature::Reset()
    {
        SetDirty(true);
        for (auto& f : _dependents)
        {
            f->Reset();
        }
    }

    size_t Feature::WarmupTime() const
    {
        size_t maxTime = 0;
        for (const auto& inputFeature : _inputFeatures)
        {
            maxTime = std::max(maxTime, inputFeature->WarmupTime());
        }
        return maxTime;
    }

    size_t Feature::ColumnDelay(int column) const
    {
        return WarmupTime();
    }

    void Feature::AddInputFeature(std::shared_ptr<Feature> inputFeature)
    {
        _inputFeatures.push_back(inputFeature);
    }

    std::vector<std::string> Feature::GetColumnDescriptions() const
    {
        using std::to_string;
        std::vector<std::string> result;
        for (int index = 0; index < NumColumns(); index++)
        {
            result.push_back(FeatureType() + "_" + to_string(index));
        }
        return result;
    }

    std::vector<std::string> Feature::GetDescription() const
    {
        std::vector<std::string> result;
        result.reserve(_inputFeatures.size() + 2);

        // get everybody I depend on
        result.push_back(Id());
        result.push_back(FeatureType());
        for (const auto& inputFeature : _inputFeatures)
        {
            result.push_back(inputFeature->Id());
        }
        return result;
    }

    const std::vector<std::shared_ptr<Feature>>& Feature::GetInputFeatures() const
    {
        return _inputFeatures;
    }

    void Feature::Serialize(std::ostream& outStream) const
    {
        bool first = true;
        for (auto s : GetDescription())
        {
            if (!first)
            {
                outStream << '\t';
            }
            else
            {
                first = false;
            }
            outStream << s;
        }
        outStream << '\n';
    }

    // searches recursively through input features and returns first InputFeature it finds
    // returns nullptr if it can't find one
    std::shared_ptr<InputFeature> Feature::FindInputFeature() const
    {
        for (const auto& inputFeature : _inputFeatures)
        {
            auto m = std::dynamic_pointer_cast<InputFeature>(inputFeature);
            if (m != nullptr)
            {
                return m;
            }
            else
            {
                return inputFeature->FindInputFeature();
            }
        }

        return nullptr;
    }

    void Feature::RegisterDeserializeFunction(std::string class_name, std::function<std::shared_ptr<Feature>(std::vector<std::string>, FeatureMap&)> create_fn)
    {
        _createTypeMap[class_name] = create_fn;
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

    std::shared_ptr<Feature> Feature::FromDescription(const std::vector<std::string>& description, FeatureMap& deserializedFeatureMap)
    {
        std::string featureId = TrimString(description[0]);
        std::string featureClass = TrimString(description[1]);

        auto createFunction = _createTypeMap[featureClass];
        if (createFunction == nullptr)
        {
            std::string error = std::string("Error deserializing feature description: unknown feature type '") + featureClass + "'";
            throw std::runtime_error(error);
        }
        return createFunction(description, deserializedFeatureMap);
    }
}
