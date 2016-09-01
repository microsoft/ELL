////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"

// stl
#include <memory>

namespace predictors
{
    LinearPredictor::LinearPredictor() : _b(0)
    {}

    LinearPredictor::LinearPredictor(uint64_t dim) : _w(dim), _b(0)
    {}

    void LinearPredictor::Reset()
    {
        _w.Reset();
        _b = 0;
    }

    double LinearPredictor::Predict(const dataset::IDataVector& dataVector) const
    {
        return dataVector.Dot(_w) + _b;
    }

    std::vector<double> LinearPredictor::GetWeightedElements(const dataset::IDataVector& dataVector) const
    {
        std::vector<double> weightedElements(_w.Size());
        dataVector.AddTo(weightedElements);
        for (size_t i = 0; i < _w.Size(); ++i)
        {
            weightedElements[i] *= _w[i];
        }
        return weightedElements;
    }

    void LinearPredictor::Scale(double scalar)
    {
        _w.Scale(scalar);
        _b *= scalar;
    }

    utilities::ObjectDescription LinearPredictor::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<LinearPredictor>("Linear predictor");
        description.AddProperty<std::vector<double>>("w", "The weights");
        description.AddProperty<decltype(_b)>("b", "The bias");
        return description;
    }

    utilities::ObjectDescription LinearPredictor::GetDescription() const
    {
        auto description = GetTypeDescription();
        description["w"] << static_cast<std::vector<double>>(_w);
        description["b"] << _b;
        return description;
    }

    void LinearPredictor::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        std::vector<double> w;
        description["w"] >> w;
        _w = w;
        description["b"] >> _b;
    }
}
