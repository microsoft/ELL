////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IirFilterFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IirFilterFeature.h"
#include "StringUtil.h"

namespace features
{
    //
    // IirFilterFeature
    //
    std::shared_ptr<IirFilterFeature> IirFilterFeature::Create(std::shared_ptr<Feature> inputFeature, std::vector<double> b, std::vector<double> a)
    {
        return Allocate({inputFeature}, b, a);
    }

    IirFilterFeature::IirFilterFeature(ctor_enable, const std::vector<std::shared_ptr<Feature>>& inputs, std::vector<double> b, std::vector<double> a) : UnaryFunctionFeature<IirFilterFeature>(inputs), _a(a), _b(b)
    {
        _filters.emplace_back(b, a);
    }

    std::vector<double> IirFilterFeature::ComputeOutput() const
    {
        const auto& row = _inputFeatures[0]->GetOutput();

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

    layers::CoordinateList IirFilterFeature::AddToModel(layers::Model& model, const std::unordered_map<std::shared_ptr<const Feature>, layers::CoordinateList>& featureOutputs) const
    {
        layers::CoordinateList inputCoordinates;
        throw std::runtime_error("Not implemented");
        return inputCoordinates;
    }

    void IirFilterFeature::AddDescription(std::vector<std::string>& description) const
    {
        using std::to_string;
        using features::to_string;

        description.push_back(to_string(_b));
        description.push_back(to_string(_a));
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