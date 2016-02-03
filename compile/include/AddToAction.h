// AddToAction.h 

#pragma once

#include "LinearOperation.h"

// types
#include "types.h"

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
    AddToAction(std::string targetVariableName);

    ///
    ///
    AddToAction(const LinearOperation& operation, std::string targetVariableName);

    /// \returns The operation that is applied to the input variable, before it is added to the target variable
    ///
    const LinearOperation& GetOperation() const;

    ///
    ///
    const std::string& GetTargetVariableName() const;

    ///
    ///
    void SetInputCoordinates(std::string inputVariableName);

    /// \returns True if the action is not a null operations, namely, if it actually modifies the target in some way
    ///
    bool IsNull() const;

    ///
    ///
    void Print(std::ostream& os) const;

private:
    LinearOperation _operation;
    std::string _inputVariableName = "";
    std::string _targetVariableName = "";
};