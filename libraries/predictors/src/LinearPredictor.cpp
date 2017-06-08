////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"
#include "DataVectorOperations.h"

// stl
#include <memory>

namespace ell
{
namespace predictors
{
    LinearPredictor::LinearPredictor()
        : _w(0), _b(0)
    {
    }

    LinearPredictor::LinearPredictor(size_t dim)
        : _w(dim), _b(0)
    {
    }

    LinearPredictor::LinearPredictor(const math::ColumnVector<double>& weights, double bias)
        : _w(weights), _b(bias)
    {
    }

    void LinearPredictor::Reset()
    {
        _w.Reset();
        _b = 0;
    }

    void LinearPredictor::Resize(size_t size)
    {
        _w.Resize(size);
    }

    double LinearPredictor::Predict(const DataVectorType& dataVector) const
    {
        return _w * dataVector + _b;
    }

    auto LinearPredictor::GetWeightedElements(const DataVectorType& dataVector) const -> DataVectorType
    {
        auto transformation = [&](data::IndexValue indexValue) -> double { return indexValue.value * _w[indexValue.index]; };
        return dataVector.TransformAs<data::IterationPolicy::skipZeros,DataVectorType>(transformation);
    }

    void LinearPredictor::Scale(double scalar)
    {
        _w *= scalar; 
        _b *= scalar;
    }

    void LinearPredictor::WriteToArchive(utilities::Archiver& archiver) const
    {
        auto w = _w.ToArray();
        archiver["w"] << w;
        archiver["b"] << _b;
    }

    void LinearPredictor::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<double> w;
        archiver["w"] >> w;
        _w = math::ColumnVector<double>(std::move(w));
        archiver["b"] >> _b;
    }
}
}
