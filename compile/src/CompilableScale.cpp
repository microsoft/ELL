////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableScale.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableScale.h"

void CompilableScale::BackwardPass(uint64 currentLayerIndex, DataFlowGraph& graph) const // TODO move this function to common coordinatewise class
{
    for(uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _coordinates[column];
        auto& inputActionList = graph[coordinate.GetRow()][coordinate.GetColumn()].Actions;
        const auto& outputActionList = graph[currentLayerIndex][column].Actions;

        //create the linear operation
        LinearOperation inputOperation(_values[column], 0.0);

        for(const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            const std::string& targetVariableName = action.GetTargetVariableName();
            inputActionList.emplace_back(outputOperation.Compound(inputOperation), targetVariableName);
        }
    }
}
