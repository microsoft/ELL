////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     LinearPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
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

    double LinearPredictor::Predict(const dataset::IDataVector& dataVector) const
    {
        return dataVector.Dot(_w) + _b;
    }

    void LinearPredictor::Scale(double scalar)
    {
        _w.Scale(scalar);
        _b *= scalar;
    }

    void LinearPredictor::AddToModel(layers::Model& model, const layers::CoordinateList& inputCoordinates) const
    {
        auto multiplyLayer = std::make_unique<layers::Coordinatewise>(_w, inputCoordinates, layers::Coordinatewise::OperationType::multiply);
        uint64_t layerIndex = model.AddLayer(std::move(multiplyLayer));

        auto coordinates = model.BuildCoordinateList(layerIndex);
        auto sumLayer = std::make_unique<layers::Sum>(coordinates);
        layerIndex = model.AddLayer(std::move(sumLayer));

        auto biasLayer = std::make_unique<layers::Coordinatewise>(layers::Coordinatewise::OperationType::add);
        biasLayer->Append(_b, layers::Coordinate{layerIndex, 0});
        model.AddLayer(std::move(biasLayer));
    }
}
