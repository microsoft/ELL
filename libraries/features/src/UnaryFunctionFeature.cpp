////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryFunctionFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryFunctionFeature.h"
#include "Feature.h"
#include "VectorMath.h"
#include "StringUtil.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace features
{
    //
    // MagnitudeFeature
    //
    std::shared_ptr<MagnitudeFeature> MagnitudeFeature::Create(std::shared_ptr<Feature> inputFeature)
    {
        return Allocate(inputFeature);
    }

    MagnitudeFeature::MagnitudeFeature(ctor_enable, std::shared_ptr<Feature> inputFeature) : UnaryFunctionFeature<MagnitudeFeature>(inputFeature)
    {}

    std::vector<double> MagnitudeFeature::ComputeValue() const
    {
        assert(_inputFeatures.size() == 1);
        const auto& in_vec = _inputFeatures[0]->Eval();
        double m = 0;
        for (auto v : in_vec)
        {
            m += v*v;
        }
        std::vector<double> result = { std::sqrt(m) };
        _numColumns = result.size();
        return result;
    }

    std::shared_ptr<Feature> MagnitudeFeature::Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures)
    {
        std::shared_ptr<Feature> inputFeature = previousFeatures[params[2]];
        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }
        return Create(inputFeature);
    }

    //
    // IirFilterFeature
    //
    std::shared_ptr<IirFilterFeature> IirFilterFeature::Create(std::shared_ptr<Feature> inputFeature, std::vector<double> b, std::vector<double> a)
    {
        return Allocate(inputFeature, b, a);
    }

    IirFilterFeature::IirFilterFeature(ctor_enable, std::shared_ptr<Feature> inputFeature, std::vector<double> b, std::vector<double> a) : UnaryFunctionFeature<IirFilterFeature>(inputFeature), _a(a), _b(b)
    {
        _filters.emplace_back(b, a);
    }

    std::vector<double> IirFilterFeature::ComputeValue() const
    {
        const auto& row = _inputFeatures[0]->Eval();

        // resize to accomodate row
        while (row.size() > _filters.size())
        {
            _filters.push_back(_filters.back());
        }

        std::vector<double> result(row.size());
        for (size_t index = 0; index < row.size(); index++)
        {
            result[index] = _filters[index].FilterSample(row[index]);
        }
        _numColumns = result.size();
        return result;
    }

    size_t IirFilterFeature::WarmupTime() const
    {
        return Feature::WarmupTime() + std::max(_a.size(), _b.size());
    }

    std::vector<std::string> IirFilterFeature::GetDescription() const
    {
        using std::to_string;
        using features::to_string;
        std::vector<std::string> result = Feature::GetDescription();

        result.push_back(to_string(_b));
        result.push_back(to_string(_a));
        return result;
    }

    std::shared_ptr<Feature> IirFilterFeature::Deserialize(std::vector<std::string> params, FeatureMap& previousFeatures)
    {
        std::shared_ptr<Feature> inputFeature = previousFeatures[params[2]];
        if (inputFeature == nullptr)
        {
            std::string error_msg = std::string("Error deserializing feature description: unknown input feature ") + params[2];
            throw std::runtime_error(error_msg);
        }

        std::vector<double> b = ParseValueListString(params[3]);
        std::vector<double> a = ParseValueListString(params[4]);

        return Create(inputFeature, b, a);
    }
}