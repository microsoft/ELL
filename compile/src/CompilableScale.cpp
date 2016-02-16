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
        auto coordinate = _inputCoordinates[column];
        auto& inputActionList = graph[coordinate.GetLayerIndex()][coordinate.GetElementIndex()].Actions;
        const auto& outputActionList = graph[currentLayerIndex][column].Actions;

        //create the linear operation
        LinearOperation inputOperation(_values[column], 0.0);

        for(const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            const auto target = action.GetTarget();
            inputActionList.emplace_back(outputOperation.Compound(inputOperation), target);
        }
    }
}
