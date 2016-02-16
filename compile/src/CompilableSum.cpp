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
    for(uint64 elementIndex = 0; elementIndex < Size(); ++elementIndex)
    {
        layers::Coordinate thisCoordinate(currentLayerIndex, elementIndex);

        auto& thisNode = graph[currentLayerIndex][elementIndex];

        // skip empty action lists
        if(thisNode.Actions.size() == 0)
        {
            continue;
        }

        for(uint64 i = 0; i < _inputCoordinates[elementIndex].size(); ++i)
        {
            auto inputCoordinate = _inputCoordinates[elementIndex][i];
            auto& inputNode = graph[inputCoordinate.GetLayerIndex()][inputCoordinate.GetElementIndex()];
            inputNode.Actions.emplace_back(thisCoordinate);
            
            // increment the input counter
            ++(thisNode.NumUncomputedInputs);
        }
    }
}