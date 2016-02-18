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

// stl
#include <vector>
#include <string>

// types
#include "types.h"

struct DataFlowNode
{
    /// <summary> The actions. </summary>
    std::vector<AddToAction> Actions;

    /// <summary> Name of the variable that represents this node. </summary>
    std::string VariableName = "";

    /// <summary> true if the variable associated with this node is initialized initialized. </summary>
    bool IsInitialized = false;

    /// <summary> Number of inputs that need to be computed before the value of this node is ready. </summary>
    uint64 NumUncomputedInputs = 0;
};
