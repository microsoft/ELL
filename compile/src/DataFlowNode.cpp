////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     DataFlowNode.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataFlowNode.h"

// stl
#include <cassert>

void DataFlowNode::SetFixedVariableName(const std::string & name)
{
    _fixedVariableName = name;
}

bool DataFlowNode::IsInitialized() const
{
    return _isInitialized;
}

void DataFlowNode::SetInitialized()
{
    _isInitialized = true;
}

bool DataFlowNode::HasTempVariableName() const
{
    if (_tempVariableIndex >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DataFlowNode::HasFixedVariableName() const
{
    if (_fixedVariableName != "")
    {
        return true;
    }
    else
    {
        return false;
    }
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
        throw std::runtime_error("this should never happen");
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
        throw std::runtime_error("this should never happen");
    }
}

void DataFlowNode::SetTempVariableIndex(uint64_t index)
{
    assert(_tempVariableIndex < 0);

    _tempVariableIndex = (int)index;
}

bool DataFlowNode::HasActions() const
{
    if (_actions.empty())
    {
        return false;
    }
    else
    {
        return true;
    }
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

const std::vector<AddToAction>& DataFlowNode::GetActions() const
{
    return _actions;
}

bool DataFlowNode::IsWaitingForInputs() const
{
    if (_numUncomputedInputs == 0)
    {
        return false;
    }
    {
        return true;
    }
}

void DataFlowNode::IncrementUncomputedInputs()
{
    ++_numUncomputedInputs;
}

void DataFlowNode::DecrementUncomputedInputs()
{
    --_numUncomputedInputs;
}

