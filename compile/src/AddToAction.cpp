// AddToAction.cpp

#include "AddToAction.h"

// stl
#include<cassert>

AddToAction::AddToAction(string targetVariableName) : _targetVariableName(targetVariableName)
{}

AddToAction::AddToAction(const LinearOperation & operation, string targetVariableName) : _operation(operation), _targetVariableName(targetVariableName)
{}

const LinearOperation& AddToAction::GetOperation() const
{
    return _operation;
}

void AddToAction::SetInputCoordinates(string inputVariableName)
{
    _inputVariableName = inputVariableName;
}

bool AddToAction::IsNull() const
{
    return _operation.IsNull();
}

void AddToAction::Print(ostream & os) const
{
    // Print is never called before SetInputCoordinates()
    assert(_inputVariableName != "");

    os << _targetVariableName << " += ";
    _operation.Print(os, _inputVariableName);
    os << ";\n";
}
