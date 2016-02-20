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

void CompilableSum::SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const
{
    for(uint64 elementIndex = 0; elementIndex < Size(); ++elementIndex)
    {
        layers::Coordinate thisCoordinate(currentLayerIndex, elementIndex);

        auto& thisNode = graph.GetNode(currentLayerIndex, elementIndex);

        // skip empty action lists
        if(thisNode.HasActions() == false)
        {
            continue;
        }

        for(uint64 i = 0; i < _inputCoordinates[elementIndex].size(); ++i)
        {
            auto inputCoordinate = _inputCoordinates[elementIndex][i];
            auto& inputNode = graph.GetNode(inputCoordinate);
            inputNode.GetActions().emplace_back(thisCoordinate);
            
            // increment the input counter
            thisNode.IncrementUncomputedInputs();
        }
    }
}