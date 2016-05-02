////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AddToAction.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AddToAction.h"

// stl
#include<cassert>

AddToAction::AddToAction(layers::Coordinate targetCoordinate) : _targetCoordinate(targetCoordinate)
{}

AddToAction::AddToAction(const LinearOperation & operation, layers::Coordinate targetCoordinate) : _operation(operation), _targetCoordinate(targetCoordinate)
{}

const LinearOperation& AddToAction::GetOperation() const
{
    return _operation;
}

const layers::Coordinate& AddToAction::GetTarget() const
{
    return _targetCoordinate;
}

bool AddToAction::IsNull() const
{
    return _operation.IsNull();
}
