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

CompilableCoordinatewise::CompilableCoordinatewise(layers::Layer::Type type) : Coordinatewise(type)
{}

void CompilableCoordinatewise::SetActions(uint64 currentLayerIndex, DataFlowGraph& graph) const 
{
    for (uint64 column = 0; column < Size(); ++column)
    {
        auto coordinate = _inputCoordinates[column];
        auto& inputNode = graph.GetNode(coordinate);
        const auto& outputActionList = graph.GetNode(currentLayerIndex, column).GetActions();

        //create the linear operation
        LinearOperation inputOperation; 
        if (_type == layers::Layer::Type::scale)
        {
            inputOperation = LinearOperation(_values[column], 0.0);
        }
        else if (_type == layers::Layer::Type::shift)
        {
            inputOperation = LinearOperation(1.0, _values[column]);
        }
        else
        {
            throw std::runtime_error("unsupported operation");
        }

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
