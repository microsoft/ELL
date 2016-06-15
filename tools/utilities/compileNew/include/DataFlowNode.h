////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataFlowNode.h (compile)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AddToAction.h"

// stl
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

/// <summary> Implement a node in a data flow graph. </summary>
class DataFlowNode
{
public:

    /// <summary> A signed a fixed variable name to this node. </summary>
    ///
    /// <param name="name"> The fixed variable name. </param>
    void SetFixedVariableName(const std::string& name);

    /// <summary> Query if this node is initialized, namely, if one of its input actions has been performed. </summary>
    ///
    /// <returns> true if the node has been initialized. </returns>
    bool IsInitialized() const { return _isInitialized; }

    /// <summary> Sets the node's state to initalized. </summary>
    void SetInitialized() { _isInitialized = true; }

    /// <summary> Query if this object has a variable name. </summary>
    ///
    /// <returns> true if the node has a fixed variable name. </returns>
    bool HasFixedVariableName() const { return _fixedVariableName != ""; }

    /// <summary> Gets the node's variable name. </summary>
    ///
    /// <returns> The variable name. </returns>
    std::string GetVariableName() const;

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
    std::string _fixedVariableName = "";
    bool _isInitialized = false;
    int _tempVariableIndex = -1;
    uint64_t _numUncomputedInputs = 0;
};
