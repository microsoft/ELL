////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableSum.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableSum.h"

void CompilableSum::BackwardPass(uint64 currentLayerIndex, DataFlowGraph& graph) const
{
    for(uint64 column = 0; column < Size(); ++column)
    {
        const auto& outputActionList = graph[currentLayerIndex][column].Actions;

        // skip empty action lists
        if(outputActionList.size() == 0)
        {
            continue;
        }

        // create new variable in the code
        auto targetName = GetNextTempVariableName();

        for(uint64 i = 0; i < _coordinates[column].size(); ++i)
        {
            auto coordinate = _coordinates[column][i];
            auto& inputActionList = graph[coordinate.GetRow()][coordinate.GetColumn()].Actions;
            inputActionList.emplace_back(targetName);
        }
    }
}