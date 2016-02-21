////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableMap.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableMap.h"
#include "CompilableInput.h"
#include "CompilableScale.h"
#include "CompilableShift.h"
#include "CompilableSum.h"
#include "DataFlowNode.h"

// utilities
#include "IntegerStack.h"
#include "StringFormat.h"

// stl
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>
namespace
{
    void ConfirmNodeHasVariableName(DataFlowNode& targetNode, layers::Coordinate targetCoordinate, utilities::IntegerStack& stack, std::ostream& os)
    {
        // check if temp variable allocation needed
        if (!targetNode.HasVariableName())
        {
            // get next available temp variable index
            uint64 tempVariableIndex = stack.Pop();

            if (stack.IsTopNovel())
            {
                auto msg = "    // allocating temporary variable %i to element (%i,%i)\n    double ";
                utilities::StringFormat(os, msg, tempVariableIndex, targetCoordinate.GetLayerIndex(), targetCoordinate.GetElementIndex());
            }
            else
            {
                auto msg = "    // reassigning temporary variable %i to element (%i,%i)\n";
                utilities::StringFormat(os, msg, tempVariableIndex, targetCoordinate.GetLayerIndex(), targetCoordinate.GetElementIndex());
            }

            // assign name from int stack
            targetNode.SetTempVariableIndex(tempVariableIndex);
        }
        else
        {
            os << "    ";
        }

    }

    void ProcessNode(const layers::Coordinate& currentCoordinate, DataFlowGraph& graph, utilities::IntegerStack& stack, std::ostream& os)
    {
        auto& currentNode = graph.GetNode(currentCoordinate);
        auto currentNodeVariableName = currentNode.GetVariableName();

        // for each action
        while(currentNode.HasActions())
        {
            // get the next action in the current node
            auto action = currentNode.PopAction();

            // get the target coordinate and target node
            auto targetCoordinate = action.GetTarget();
            auto& targetNode = graph.GetNode(targetCoordinate);

            // confirm that the target node as a variable name
            ConfirmNodeHasVariableName(targetNode, targetCoordinate, stack, os);

            // print variable name
            os << targetNode.GetVariableName() << ' ';

            // print either '=' or '+='
            if(targetNode.IsInitialized())
            {
                os << "+= ";
            }
            else
            {
                os << "= ";
                targetNode.SetInitialized();
            }

            // print the right hand side
            action.GetOperation().Print(currentNodeVariableName, os);
            os << ";\n";

            // check if temp variable can be released
            if(currentNode.HasActions() == false && currentNode.HasTempVariableName())
            {
                stack.Push(currentNode.GetTempVariableIndex());
            }

            // if target node has all of its inputs, process it 
            targetNode.DecrementUncomputedInputs();
            if(targetNode.IsWaitingForInputs() == false)
            {
                ProcessNode(targetCoordinate, graph, stack, os);
            }
        }
    }
}

void CompilableMap::ToCode(layers::CoordinateList coordinateList, std::ostream& os) const
{
    // create data flow graph datastructure
    DataFlowGraph graph;
    for(uint64 layerIndex = 0; layerIndex < NumLayers(); ++layerIndex)
    {
        graph.AddLayer(_layers[layerIndex]->Size());
    }

    // set names on input layer
    uint64 inputLayerSize = _layers[0]->Size();
    const std::string inputFixedVariableName = "input";
    for (uint64 elementIndex = 0; elementIndex < inputLayerSize; ++elementIndex)
    {
        graph.GetNode(0, elementIndex).SetFixedVariableName(inputFixedVariableName + "[" + std::to_string(elementIndex) + "]");
    }

    // add extra layer for outputs
    uint64 outputLayerSize = coordinateList.size();
    uint64 outputLayerIndex = NumLayers();
    graph.AddLayer(outputLayerSize);

    // set names on output layer and set the actions that generate the output coordinates
    const std::string outputFixedVariableName = "output";
    for (uint64 outputElementIndex = 0; outputElementIndex < outputLayerSize; ++outputElementIndex)
    {
        auto inputCoordinate = coordinateList[outputElementIndex];
        layers::Coordinate outputCoordinate(outputLayerIndex, outputElementIndex);
        graph.GetNode(outputCoordinate).SetFixedVariableName(outputFixedVariableName + "[" + std::to_string(outputElementIndex) + "]");
        graph.GetNode(inputCoordinate).EmplaceAction(outputCoordinate);
    }

    // backwards pass to assign actions to nodes
    for(uint64 layerIndex = NumLayers() - 1; layerIndex > 0; --layerIndex)
    {
        auto compilableLayer = GetLayer<CompilableLayer>(layerIndex);
        compilableLayer->SetActions(layerIndex, graph);
    }

    // construct an integer stack, to manage temp variable names;
    utilities::IntegerStack stack;

    // print function declaration
    auto str = "// Predict function\n// input dimension is %i\n// output dimension is %i\nvoid Predict(const double* input, double* output)\n{\n";
    utilities::StringFormat(os, str, inputLayerSize, outputLayerSize);

    // forwards pass to generate code
    for (uint64 inputElementIndex = 0; inputElementIndex < inputLayerSize; ++inputElementIndex)
    {
        layers::Coordinate inputCoordinate(0, inputElementIndex);
        ProcessNode(inputCoordinate, graph, stack, os);
    }

    os << "}\n";
}

void CompilableMap::Deserialize(utilities::JsonSerializer& serializer)
{
    serializer.Read("layers", _layers, CompilableMap::DeserializeLayers);
}

void CompilableMap::DeserializeLayers(utilities::JsonSerializer& serializer, std::shared_ptr<layers::Layer>& up)
{
    auto type = serializer.Read<std::string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Input")
    {
        auto upZero = std::make_shared<CompilableInput>();
        upZero->Deserialize(serializer, version);
        up = upZero;
    }
    else if (type == "Scale")
    {
        auto upScale = std::make_shared<CompilableScale>();
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = std::make_shared<CompilableShift>();
        upShift->Deserialize(serializer, version);
        up = upShift;
    }
    else if (type == "Sum")
    {
        auto upSum = std::make_shared<CompilableSum>();
        upSum->Deserialize(serializer, version);
        up = upSum;
    }
    else
    {
        throw std::runtime_error("unidentified type in map file: " + type);
    }
}
