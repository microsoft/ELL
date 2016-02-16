////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     AddToAction.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LinearOperation.h"

// types
#include "types.h"

// layers
#include "Coordinate.h"

// stl
#include <string>
#include <iostream>

/// Encodes the operation: "variable_targetID += a*x + b"
///
class AddToAction
{
public:
    ///
    ///
    AddToAction(layers::Coordinate targetCoordinate);

    ///
    ///
    AddToAction(const LinearOperation& operation, layers::Coordinate targetCoordinate);

    /// \returns The operation that is applied to the input variable, before it is added to the target variable
    ///
    const LinearOperation& GetOperation() const;

    ///
    ///
    const layers::Coordinate& GetTarget() const;


    /// \returns True if the action is a null operation, namely, if it does not modify the target in any way
    ///
    bool IsNull() const;

    ///
    ///
   // void Print(std::ostream& os) const;

private:
    LinearOperation _operation;
    layers::Coordinate _targetCoordinate;
};