////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"

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
        return dataVector.Dot(_w) + _b;
    }

    auto LinearPredictor::GetWeightedElements(const DataVectorType& dataVector) const -> DataVectorType
    {
        auto mapper = [&](data::IndexValue indexValue) -> double { return indexValue.value * _w[indexValue.index]; };
        return dataVector.DeepCopyAs<DataVectorType>(mapper);
    }

    void LinearPredictor::Scale(double scalar)
    {
        math::Operations::Multiply(scalar, _w);
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
