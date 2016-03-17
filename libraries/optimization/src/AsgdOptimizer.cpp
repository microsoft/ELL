////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     AsgdOptimizer.cpp (optimization)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsgdOptimizer.h"

optimization::AsgdOptimizer::AsgdOptimizer(uint64_t dim) : _total_iterations(1), _lastPredictor(dim), _averagedPredictor(dim) // start with 1 to prevent divide-by-zero
{}

const predictors::LinearPredictor& optimization::AsgdOptimizer::GetPredictor() const
{
    return _averagedPredictor;
}
