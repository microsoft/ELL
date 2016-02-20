////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataFlowNode.h (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AddToAction.h"

// types
#include "types.h"

// stl
#include <vector>
#include <string>
#include <stdexcept>

class DataFlowNode
{

public:


    void SetFixedVariableName(const std::string& name);

    bool IsInitialized() const;

    void SetInitialized();

    bool HasVariableName() const;

    std::string GetVariableName() const;

    uint64 GetTempVariableIndex() const;

    bool HasTempVariableName() const;

    void SetTempVariableIndex(uint64 index);

    bool HasActions() const;

    const std::vector<AddToAction>& GetActions() const;

    std::vector<AddToAction>& GetActions();

    bool HasUncomputedInputs() const;

    void IncrementUncomputedInputs();

    void DecrementUncomputedInputs();



    // TODO, make members private and add accessors

private:
    std::vector<AddToAction> _actions;
    std::string _fixedVariableName = "";
    bool _isInitialized = false;
    int _tempVariableIndex = -1;
    uint64 _numUncomputedInputs = 0;
};
