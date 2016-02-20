////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableShift.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableShift.h"

void CompilableShift::SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const
{
    for(uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _inputCoordinates[column];
        auto& inputActionList = graph.GetNode(coordinate).GetActions();
        const auto& outputActionList = graph.GetNode(currentLayerIndex, column).GetActions();

        //create the linear operation
        LinearOperation inputOperation(1.0, _values[column]);

        for(const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            LinearOperation compoundOperation = outputOperation.Compound(inputOperation);
            const auto target = action.GetTarget();
            if (!compoundOperation.IsNull())
            {
                inputActionList.emplace_back(compoundOperation, target);
            }
            else
            {
                graph.GetNode(target).DecrementUncomputedInputs();
            }
        }
    }
}
