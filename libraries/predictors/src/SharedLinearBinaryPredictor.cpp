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
#include "CoordinateListTools.h"
#include "Sum.h"

// stl
#include <memory>

namespace predictors
{
    SharedLinearBinaryPredictor::BiasedVector::BiasedVector(uint64 dim) : w(dim), b(0.0)
    {}

    SharedLinearBinaryPredictor::SharedLinearBinaryPredictor(uint64 dim)
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

    void SharedLinearBinaryPredictor::AddToMap(layers::Map& map, layers::CoordinateList inputCoordinates) const
    {
        if (inputCoordinates.size() == 0)
        {
            inputCoordinates = layers::GetCoordinateList(map, map.NumLayers() - 1);
        }

        if (inputCoordinates.size() != _sp_predictor->w.Size())
        {
            throw std::runtime_error("Error in SharedLinearBinaryPredictor: input coordinates size doesn't match weight vector");
        }

        uint64 layerIndex = map.AddLayer(std::make_unique<layers::Coordinatewise>(_sp_predictor->w, inputCoordinates, layers::Coordinatewise::OperationType::multiply));
        auto coordinates = layers::GetCoordinateList(map, layerIndex);
        layerIndex = map.AddLayer(std::make_unique<layers::Sum>(coordinates));
        map.AddLayer(std::make_unique<layers::Coordinatewise>(_sp_predictor->b, layers::Coordinate{layerIndex, 0}, layers::Coordinatewise::OperationType::add));
    }
}
