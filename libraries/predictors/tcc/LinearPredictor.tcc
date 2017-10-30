////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearPredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"
#include "DataVectorOperations.h"

// stl
#include <memory>

// math
#include "VectorOperations.h"

namespace ell
{
namespace predictors
{
    template <typename ElementType>
    LinearPredictor<ElementType>::LinearPredictor(size_t dim)
        : _w(dim), _b(0)
    {
    }

    template <typename ElementType>
    LinearPredictor<ElementType>::LinearPredictor(const math::ColumnVector<ElementType>& weights, ElementType bias)
        : _w(weights), _b(bias)
    {
    }

    template <typename ElementType>
    template <typename OtherElementType>
    LinearPredictor<ElementType>::LinearPredictor(const LinearPredictor<OtherElementType>& other)
        : _b(other.GetBias())
    {
        auto weights = other.GetWeights();
        _w.Resize(weights.Size());
        for (size_t i = 0; i < weights.Size(); ++i)
        {
            _w[i] = static_cast<ElementType>(weights[i]);
        }
    }

    template <typename ElementType>
    void LinearPredictor<ElementType>::Reset()
    {
        _w.Reset();
        _b = 0;
    }

    template <typename ElementType>
    void LinearPredictor<ElementType>::Resize(size_t size)
    {
        _w.Resize(size);
    }

    template <typename ElementType>
    ElementType LinearPredictor<ElementType>::Predict(const DataVectorType& dataVector) const
    {
        return _w * dataVector + _b;
    }

    template <typename ElementType>
    auto LinearPredictor<ElementType>::GetWeightedElements(const DataVectorType& dataVector) const -> DataVectorType
    {
        auto transformation = [&](data::IndexValue indexValue) -> ElementType { return indexValue.value * _w[indexValue.index]; };
        return dataVector.TransformAs<data::IterationPolicy::skipZeros,DataVectorType>(transformation);
    }

    template <typename ElementType>
    void LinearPredictor<ElementType>::Scale(ElementType scalar)
    {
        _w *= scalar; 
        _b *= scalar;
    }

    template <typename ElementType>
    void LinearPredictor<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        auto w = _w.ToArray();
        archiver["w"] << w;
        archiver["b"] << _b;
    }

    template <typename ElementType>
    void LinearPredictor<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<ElementType> w;
        archiver["w"] >> w;
        _w = math::ColumnVector<ElementType>(std::move(w));
        archiver["b"] >> _b;
    }
}
}
