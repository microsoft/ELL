#include "SharedLinearBinaryPredictor.h"

#include "Coordinate.h"
using layers::Coordinate;

#include "Scale.h"
using layers::Scale;

#include "Sum.h"
using layers::Sum;

#include "Shift.h"
using layers::Shift;

#include <memory>
using std::make_shared;

namespace predictors
{
    SharedLinearBinaryPredictor::BiasedVector::BiasedVector(uint64 dim) : w(dim), b(0.0)
    {}

    SharedLinearBinaryPredictor::SharedLinearBinaryPredictor(uint64 dim)
    {
        _sp_predictor = make_shared<BiasedVector>(dim);
    }

    DoubleVector & SharedLinearBinaryPredictor::GetVector()
    {
        return _sp_predictor->w;
    }

    const DoubleVector & SharedLinearBinaryPredictor::GetVector() const
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

    void SharedLinearBinaryPredictor::AddTo(Map& map, const CoordinateList& inputCoordinates) const
    {
        uint64 rowIndex = map.PushBack(make_shared<Scale>(_sp_predictor->w, inputCoordinates));
        auto coordinates = map.GetCoordinateList(rowIndex);
        rowIndex = map.PushBack(make_shared<Sum>(coordinates));

        map.PushBack(make_shared<Shift>(_sp_predictor->b, Coordinate{rowIndex, 0}));
    }
}
