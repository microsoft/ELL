////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     CompilableCoordinatewise.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableCoordinatewise.h"

uint64_t CompilableCoordinatewise::GetOutputDimension() const
{
    return Coordinatewise::GetOutputDimension();
}

void CompilableCoordinatewise::operator=(const layers::Coordinatewise& coordinatewise)
{
    Coordinatewise::operator=(coordinatewise);
}

void CompilableCoordinatewise::operator=(const layers::Layer& layer)
{
    operator=(dynamic_cast<const layers::Coordinatewise&>(layer));
}

void CompilableCoordinatewise::SetActions(uint64_t currentLayerIndex, DataFlowGraph& graph) const 
{
    for (uint64_t column = 0; column < GetOutputDimension(); ++column)
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
