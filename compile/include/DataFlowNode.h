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

struct DataFlowNode
{

public:


    bool HasVariableName() const
    {
        if(FixedVariableName != "" || TempVariableIndex >= 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::string GetVariableName() const
    {
        if(FixedVariableName != "")
        {
            return FixedVariableName;
        }
        else if(TempVariableIndex >= 0)
        {
            return "tmp" + std::to_string(TempVariableIndex);
        }
        else
        {
            throw std::runtime_error("this should never happen");
        }
    }

    uint64 GetTempVariableIndex() const
    {
        if(TempVariableIndex >= 0)
        {
            return (uint64)TempVariableIndex;
        }
        else
        {
            throw std::runtime_error("this should never happen");
        }
    }

    void SetTempVariableIndex(uint64 index)
    {
        TempVariableIndex = (int)index;
    }

    void SetFixedFixedVariableName(const std::string& name)
    {
        FixedVariableName = name;
    }

    // TODO, make members private and add accessors

    /// <summary> The actions. </summary>
    std::vector<AddToAction> Actions;

    /// <summary> Name of the variable that represents this node. </summary>
    std::string FixedVariableName = "";

    /// <summary> true if the variable associated with this node is initialized initialized. </summary>
    bool IsInitialized = false;

    int TempVariableIndex = -1;

    /// <summary> Number of inputs that need to be computed before the value of this node is ready. </summary>
    uint64 NumUncomputedInputs = 0;
};
