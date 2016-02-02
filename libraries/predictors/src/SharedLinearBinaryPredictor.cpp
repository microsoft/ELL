#include "SharedLinearBinaryPredictor.h"

// layers
#include "Coordinate.h"
#include "Scale.h"
#include "Sum.h"
#include "Shift.h"

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

    void SharedLinearBinaryPredictor::AddTo(layers::Map& map, const layers::CoordinateList& inputCoordinates) const
    {
        uint64 rowIndex = map.PushBack(std::make_shared<layers::Scale>(_sp_predictor->w, inputCoordinates));
        auto coordinates = map.GetCoordinateList(rowIndex);
        rowIndex = map.PushBack(std::make_shared<layers::Sum>(coordinates));

        map.PushBack(std::make_shared<layers::Shift>(_sp_predictor->b, layers::Coordinate{rowIndex, 0}));
    }
}
