////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SequentialOptimizer.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SequentialOptimizer.h"

#include <utilities/include/Exception.h>

using namespace ell::model::optimizer;

void SequentialOptimizer::Reset()
{
    _currentTransformation = _transformations.begin();
}

bool SequentialOptimizer::IsDone() const
{
    return _currentTransformation == _transformations.end();
}

bool SequentialOptimizer::KeepTransformation(const Objective::ObjectiveValue& objectiveDelta) const
{
    return objectiveDelta >= 0;
}

const Transformation& SequentialOptimizer::GetTransformation()
{
    auto it = _currentTransformation;
    ++_currentTransformation;
    return **it;
}
