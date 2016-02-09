////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     AddToAction.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AddToAction.h"

// stl
#include<cassert>

AddToAction::AddToAction(std::string targetVariableName) : _targetVariableName(targetVariableName)
{}

AddToAction::AddToAction(const LinearOperation & operation, std::string targetVariableName) : _operation(operation), _targetVariableName(targetVariableName)
{}

const LinearOperation& AddToAction::GetOperation() const
{
    return _operation;
}

const std::string & AddToAction::GetTargetVariableName() const
{
    return _targetVariableName;
}

void AddToAction::SetInputCoordinates(std::string inputVariableName)
{
    _inputVariableName = inputVariableName;
}

bool AddToAction::IsNull() const
{
    return _operation.IsNull();
}

void AddToAction::Print(std::ostream & os) const
{
    // Print is never called before SetInputCoordinates()
    assert(_inputVariableName != "");

    os << _targetVariableName << " += ";
    _operation.Print(os, _inputVariableName);
    os << ";\n";
}
