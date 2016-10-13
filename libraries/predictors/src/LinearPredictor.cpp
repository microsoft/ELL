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
        auto mapper = [&](data::IndexValue indexValue)->double { return indexValue.value * _w[indexValue.index]; };
        return dataVector.Duplicate<DataVectorType>(mapper);
    }

    void LinearPredictor::Scale(double scalar)
    {
        // _w.Scale(scalar); // TODO call Operation
        _b *= scalar;
    }

    void LinearPredictor::WriteToArchive(utilities::Archiver& archiver) const
    {
        // archiver["w"] << static_cast<std::vector<double>>(_w); TODO Vector serialization
        archiver["b"] << _b;
    }

    void LinearPredictor::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<double> w;
        archiver["w"] >> w;
        // _w = w; TODO Serialization
        archiver["b"] >> _b;
    }
}
}
