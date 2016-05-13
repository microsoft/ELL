////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"

// layers
#include "Coordinate.h"
#include "Coordinatewise.h"
#include "CoordinateListTools.h"
#include "Sum.h"

// stl
#include <memory>

namespace predictors
{
    LinearPredictor::LinearPredictor(uint64_t dim) : _w(dim), _b(0)
    {}

    linear::DoubleVector& LinearPredictor::GetVector()
    {
        return _w;
    }

    const linear::DoubleVector& LinearPredictor::GetVector() const
    {
        return _w;
    }

    double& LinearPredictor::GetBias()
    {
        return _b;
    }

    double LinearPredictor::GetBias() const
    {
        return _b;
    }

    uint64_t LinearPredictor::GetDimension() const
    {
        return _w.Size();
    }

    void LinearPredictor::Reset()
    {
        _w.Reset();
        _b = 0;
    }

    double LinearPredictor::Predict(const dataset::IDataVector& dataVector) const
    {
        return dataVector.Dot(_w) + _b;
    }

    void LinearPredictor::Scale(double scalar)
    {
        _w.Scale(scalar);
        _b *= scalar;
    }

    void LinearPredictor::AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const
    {
        auto weightsLayer = std::make_unique<layers::Coordinatewise>(std::vector<double>(_w), std::move(inputCoordinates), layers::Coordinatewise::OperationType::multiply);
        auto weightsLayerCoordinates = model.AddLayer(std::move(weightsLayer));

        auto sumLayer = std::make_unique<layers::Sum>(std::move(weightsLayerCoordinates));
        auto sumLayerCoordinates = model.AddLayer(std::move(sumLayer));

        auto biasLayer = std::make_unique<layers::Coordinatewise>(_b, sumLayerCoordinates[0], layers::Coordinatewise::OperationType::add);
        model.AddLayer(std::move(biasLayer));
    }

    void LinearPredictor::Swap(LinearPredictor& u, LinearPredictor& v)
    {
        linear::DoubleVector::Swap(u._w, v._w);
        std::swap(u._b, v._b);
    }
}
