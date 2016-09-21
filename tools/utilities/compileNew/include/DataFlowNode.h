////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataFlowNode.h (compile)
//  Authors:  Ofer Dekel, Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AddToAction.h"

// stl
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

class ScalarVariable
{
public:
	ScalarVariable();
	ScalarVariable(std::string name);
	ScalarVariable(std::string name, uint64_t elementOffset);

	const std::string& Name() const 
	{
		return _name;
	}
	bool HasName() const
	{
		return !_name.empty();
	}
	uint64_t ElementOffset() const
	{
		return _elementOffset;
	}
	bool IsArrayElement() const
	{
		return _isArray;
	}

	const std::string& EmittedName()
	{
		return _emittedName;
	}
	bool HasEmittedName() const
	{
		return !_emittedName.empty();
	}

	void Set(std::string name);
	void Set(std::string name, const uint64_t elementOffset);

	void SetEmittedName(std::string name);

private:
	void SetName(std::string&& name);

private:
	bool _isArray = false;
	std::string _name;
	uint64_t _elementOffset = 0;
	std::string _emittedName;
};

/// <summary> Implement a node in a data flow graph. </summary>
class DataFlowNode
{
public:

    /// <summary> A signed a fixed variable name to this node. </summary>
    ///
    /// <param name="name"> The fixed variable name. </param>
    void SetFixedVariableName(const std::string& name);	
	void SetFixedVariableName(const std::string& name, uint64_t arrayOffset);

    /// <summary> Query if this node is initialized, namely, if one of its input actions has been performed. </summary>
    ///
    /// <returns> true if the node has been initialized. </returns>
    bool IsInitialized() const { return _isInitialized; }

    /// <summary> Sets the node's state to initalized. </summary>
    void SetInitialized() { _isInitialized = true; }

    /// <summary> Query if this object has a variable name. </summary>
    ///
    /// <returns> true if the node has a fixed variable name. </returns>
    bool HasFixedVariableName() const 
	{ 
		return _isFixed; 
	}

    /// <summary> Gets the node's variable name. </summary>
    ///
    /// <returns> The variable name. </returns>
	ScalarVariable& Variable()
	{
		return _variable;
	}

    /// <summary> Gets the node's temporary variable index. </summary>
    ///
    /// <returns> The temporary variable index. </returns>
    uint64_t GetTempVariableIndex() const;

    /// <summary> Query if this object has a temporary variable name. </summary>
    ///
    /// <returns> true if the node has a temporary variable name. </returns>
    bool HasTempVariableName() const { return _tempVariableIndex >= 0; }

    /// <summary> Sets the node's temporary variable index. </summary>
    ///
    /// <param name="index"> The node's temporary variable index. </param>
    void SetTempVariableIndex(uint64_t index);

    /// <summary> Query if this node has actions. </summary>
    ///
    /// <returns> true if the node actions. </returns>
    bool HasActions() const { return !_actions.empty(); }

    /// <summary> Pops an action of the node's action stack. </summary>
    ///
    /// <returns> An AddToAction. </returns>
    AddToAction PopAction();

    /// <summary> Emplaces an action in the node's action stack. </summary>
    ///
    /// <param name="targetCoordinate"> The target coordinate of the action being added. </param>
    void EmplaceAction(layers::Coordinate targetCoordinate);

    /// <summary> Emplaces an action in the node's action stack. </summary>
    ///
    /// <param name="operation"> The actions's operation. </param>
    /// <param name="targetCoordinate"> The target coordinate of the action being added. </param>
    void EmplaceAction(const LinearOperation& operation, layers::Coordinate targetCoordinate);

    /// <summary> Gets the vector of actions associated with this node. </summary>
    ///
    /// <returns> The vector of actions associated with this node. </returns>
    const std::vector<AddToAction>& GetActions() const { return _actions; }

    /// <summary> Query if this node is still waiting for inputs. </summary>
    ///
    /// <returns> true if the node is still waiting for more inputs. </returns>
    bool IsWaitingForInputs() const { return _numUncomputedInputs > 0; }

    /// <summary> Increment the number of inputs tha the node is still waiting for. </summary>
    void IncrementUncomputedInputs() { ++_numUncomputedInputs; }

    /// <summary> Decrement the number of inputs tha the node is still waiting for. </summary>
    void DecrementUncomputedInputs();

private:
    std::vector<AddToAction> _actions;
	ScalarVariable _variable;
	bool _isFixed = false;
    bool _isInitialized = false;
    int _tempVariableIndex = -1;
    uint64_t _numUncomputedInputs = 0;
};
