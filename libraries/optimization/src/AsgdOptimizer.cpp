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

optimization::AsgdOptimizer::AsgdOptimizer(uint64 dim) : _total_iterations(1), _w(dim), _b(0.0), _predictor(dim) // start with 1 to prevent divide-by-zero
{}

const predictors::LinearPredictor& optimization::AsgdOptimizer::GetPredictor() const
{
    return _predictor;
}
