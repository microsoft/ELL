////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SharedLinearBinaryPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SharedLinearBinaryPredictor.h"

// layers
#include "Coordinate.h"
#include "Coordinatewise.h"
#include "Sum.h"

// stl
#include <memory>

namespace predictors
{
    SharedLinearBinaryPredictor::BiasedVector::BiasedVector(uint64_t dim) : w(dim), b(0.0)
    {}

    SharedLinearBinaryPredictor::SharedLinearBinaryPredictor(uint64_t dim)
    {
        _sp_predictor = std::make_shared<BiasedVector>(dim);
    }

    linear::DoubleVector & SharedLinearBinaryPredictor::GetVector()
    {
        return _sp_predictor->w;
    }

    const linear::DoubleVector & SharedLinearBinaryPredictor::GetVector() const
    {
        return _sp_predictor->w;
    }

    double & SharedLinearBinaryPredictor::GetBias()
    {
        return _sp_predictor->b;
    }

    double SharedLinearBinaryPredictor::GetBias() const
    {
        return _sp_predictor->b;
    }

    void SharedLinearBinaryPredictor::AddToStack(layers::Stack& stack, const layers::CoordinateList& inputCoordinates) const
    {
        uint64_t layerIndex = stack.AddLayer(std::make_unique<layers::Coordinatewise>(_sp_predictor->w, inputCoordinates, layers::Coordinatewise::OperationType::multiply));
        auto coordinates = stack.GetCoordinateList(layerIndex);
        layerIndex = stack.AddLayer(std::make_unique<layers::Sum>(coordinates));
        layerIndex = stack.AddLayer(std::make_unique<layers::Coordinatewise>(_sp_predictor->b, layers::Coordinate{ layerIndex, 0 }, layers::Coordinatewise::OperationType::add));
    }
}
