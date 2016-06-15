////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AddToAction.cpp (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AddToAction.h"

// stl
#include<cassert>

AddToAction::AddToAction(layers::Coordinate targetCoordinate) : _targetCoordinate(targetCoordinate)
{}

AddToAction::AddToAction(const LinearOperation & operation, layers::Coordinate targetCoordinate) : _operation(operation), _targetCoordinate(targetCoordinate)
{}
