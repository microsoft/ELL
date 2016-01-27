// AddToAction.h 

#pragma once

#include "LinearOperation.h"

// types
#include "types.h"

// stl
#include <string>
using std::string;

#include <iostream>
using std::ostream;

/// Encodes the operation: "variable_targetID += a*x + b"
///
class AddToAction
{
public:
    ///
    ///
    AddToAction(string targetVariableName);

    ///
    ///
    AddToAction(const LinearOperation& operation, string targetVariableName);

    /// \returns The operation that is applied to the input variable, before it is added to the target variable
    ///
    const LinearOperation& GetOperation() const;

    ///
    ///
    const string& GetTargetVariableName() const;

    ///
    ///
    void SetInputCoordinates(string inputVariableName);

    /// \returns True if the action is not a null operations, namely, if it actually modifies the target in some way
    ///
    bool IsNull() const;

    ///
    ///
    void Print(ostream& os) const;

private:
    LinearOperation _operation;
    string _inputVariableName = "";
    string _targetVariableName = "";
};