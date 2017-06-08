////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MeanCalculator.cpp (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MeanCalculator.h"

namespace ell
{
namespace trainers
{
    math::RowVector<double> CalculateMean(const data::AnyDataset& anyDataset)
    {
        return CalculateTransformedMean<data::IterationPolicy::skipZeros>(anyDataset, [](data::IndexValue x) {return x.value; });
    }
}
}
