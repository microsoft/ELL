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

void CompilableShift::BackwardPass(uint64 currentLayerIndex, DataFlowGraph& graph) const
{
    for(uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _coordinates[column];
        auto& inputActionList = graph[coordinate.GetRow()][coordinate.GetColumn()].Actions;
        const auto& outputActionList = graph[currentLayerIndex][column].Actions;

        //create the linear operation
        LinearOperation inputOperation(1.0, _values[column]);

        for(const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            const std::string& targetVariableName = action.GetTargetVariableName();
            inputActionList.emplace_back(outputOperation.Compound(inputOperation), targetVariableName);
        }
    }
}
