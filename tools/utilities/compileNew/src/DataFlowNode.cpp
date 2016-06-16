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

ScalarVariable::ScalarVariable()
{
}

ScalarVariable::ScalarVariable(std::string name)
	: _name(std::move(name))
{
}

ScalarVariable::ScalarVariable(std::string name, uint64_t elementOffset)
	: _name(std::move(name)), _elementOffset(elementOffset), _isArray(true)
{
}

void ScalarVariable::Set(std::string name)
{
	SetName(std::move(name));
	_elementOffset = 0;
	_isArray = false;
}

void ScalarVariable::Set(std::string name, const uint64_t elementOffset)
{
	SetName(std::move(name));
	_elementOffset = elementOffset;
	_isArray = true;
}

void ScalarVariable::SetName(std::string&& name)
{
	_name = std::move(name);
	_emittedName.clear();
}

void ScalarVariable::SetEmittedName(std::string name)
{
	_emittedName = std::move(name);
}

void DataFlowNode::SetFixedVariableName(const std::string& name)
{
	_variable.Set(name);
	_isFixed = true;
	_tempVariableIndex = -1;
}

void DataFlowNode::SetFixedVariableName(const std::string& name, uint64_t arrayOffset)
{
	_variable.Set(name, arrayOffset);
	_isFixed = true;
	_tempVariableIndex = -1;
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
	_variable.Set("tmp" + std::to_string(_tempVariableIndex));
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

