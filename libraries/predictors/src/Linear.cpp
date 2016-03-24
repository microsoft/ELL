////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Linear.cpp (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Linear.h"

// layers
#include "Coordinate.h"
#include "Coordinatewise.h"
#include "CoordinateListTools.h"
#include "Sum.h"

// stl
#include <memory>

namespace predictors
{
    Linear::Linear(uint64 dim) : _w(dim), _b(0)
    {}

    linear::DoubleVector& Linear::GetVector()
    {
        return _w;
    }

    const linear::DoubleVector& Linear::GetVector() const
    {
        return _w;
    }

    double& Linear::GetBias()
    {
        return _b;
    }

    double Linear::GetBias() const
    {
        return _b;
    }

    double Linear::Predict(const dataset::IDataVector& dataVector) const
    {
        return dataVector.Dot(_w) + _b;
    }

    void Linear::AddToStack(layers::Stack& stack, const layers::CoordinateList& inputCoordinates) const
    {
        uint64 layerIndex = stack.AddLayer(std::make_unique<layers::Coordinatewise>(_w, inputCoordinates, layers::Coordinatewise::OperationType::multiply));
        auto coordinates = stack.GetCoordinateList(layerIndex);
        layerIndex = stack.AddLayer(std::make_unique<layers::Sum>(coordinates));
        layerIndex = stack.AddLayer(std::make_unique<layers::Coordinatewise>(_b, layers::Coordinate{ layerIndex, 0 }, layers::Coordinatewise::OperationType::add));
    }
}
