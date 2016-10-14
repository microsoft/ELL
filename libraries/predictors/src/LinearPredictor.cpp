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

namespace emll
{
namespace predictors
{
    LinearPredictor::LinearPredictor()
        : _b(0)
    {
    }

    LinearPredictor::LinearPredictor(uint64_t dim)
        : _w(dim), _b(0)
    {
    }

    void LinearPredictor::Reset()
    {
        _w.Reset();
        _b = 0;
    }

    double LinearPredictor::Predict(const DataVectorType& dataVector) const
    {
        return dataVector.Dot(_w.GetDataPointer()) + _b;
    }

    std::vector<double> LinearPredictor::GetWeightedElements(const DataVectorType& dataVector) const
    {
        std::vector<double> weightedElements(_w.Size());
        dataVector.AddTo(weightedElements.data());
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

    void LinearPredictor::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["w"] << static_cast<std::vector<double>>(_w);
        archiver["b"] << _b;
    }

    void LinearPredictor::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<double> w;
        archiver["w"] >> w;
        _w = w;
        archiver["b"] >> _b;
    }
}
}
