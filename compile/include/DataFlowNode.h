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

// types
#include "types.h"

struct DataFlowNode
{
    /// <summary> The actions. </summary>
    std::vector<AddToAction> Actions;

    /// <summary> Number of inputs that need to be computed before the value of this node is ready. </summary>
    uint64 NumUncomputedInputs = 0;

    /// <summary> Number of input actions that are ready but cannot yet be performed because this node is not allocated. </summary>
    //uint64 NumBlockedInputs = 0;

};
