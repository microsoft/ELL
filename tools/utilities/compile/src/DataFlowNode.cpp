////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataFlowNode.cpp (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataFlowNode.h"

// stl
#include <cassert>

// utilities
#include "Exception.h"

void DataFlowNode::SetFixedVariableName(const std::string & name)
{
    _fixedVariableName = name;
}

std::string DataFlowNode::GetVariableName() const
{
    if (_fixedVariableName != "")
    {
        return _fixedVariableName;
    }
    else if (_tempVariableIndex >= 0)
    {
        return "tmp" + std::to_string(_tempVariableIndex);
    }
    else
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
    }
}

uint64_t DataFlowNode::GetTempVariableIndex() const
{
    if (_tempVariableIndex >= 0)
    {
        return (uint64_t)_tempVariableIndex;
    }
    else
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
    }
}

void DataFlowNode::SetTempVariableIndex(uint64_t index)
{
    assert(_tempVariableIndex < 0);

    _tempVariableIndex = (int)index;
}

AddToAction DataFlowNode::PopAction()
{
    AddToAction last = std::move(_actions.back());
    _actions.pop_back();
    return last;
}

void DataFlowNode::EmplaceAction(layers::Coordinate targetCoordinate)
{
    _actions.emplace_back(targetCoordinate);
}

void DataFlowNode::EmplaceAction(const LinearOperation& operation, layers::Coordinate targetCoordinate)
{
    _actions.emplace_back(operation, targetCoordinate);
}

void DataFlowNode::DecrementUncomputedInputs()
{
    assert(_numUncomputedInputs > 0);
    --_numUncomputedInputs;
}

