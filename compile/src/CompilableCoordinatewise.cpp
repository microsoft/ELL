////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableCoordinatewise.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableCoordinatewise.h"

uint64 CompilableCoordinatewise::Size() const
{
    return Coordinatewise::Size();
}

void CompilableCoordinatewise::operator=(const layers::Coordinatewise& coordinatewise)
{
    Coordinatewise::operator=(coordinatewise);
}

void CompilableCoordinatewise::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::Coordinatewise&>(layer));
}

void CompilableCoordinatewise::SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const
{
    for (uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _inputCoordinates[column];
        auto& inputNode = graph.GetNode(coordinate);
        const auto& outputActionList = graph.GetNode(currentLayerIndex, column).GetActions();

        //create the linear operation
        LinearOperation inputOperation(GetOperationType(), _values[column]); 

        for (const auto& action : outputActionList)
        {
            const LinearOperation& outputOperation = action.GetOperation();
            LinearOperation compoundOperation = outputOperation.Compound(inputOperation);
            const auto target = action.GetTarget();
            if (!compoundOperation.IsNull())
            {
                inputNode.EmplaceAction(compoundOperation, target);
            }
            else
            {
                graph.GetNode(target).DecrementUncomputedInputs();
            }
        }
    }
}
