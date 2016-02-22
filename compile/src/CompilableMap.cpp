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
            if (stack.IsTopNovel())
            {
                auto msg = "    // allocating temporary variable for coordinate (%i,%i)\n    double ";
                utilities::StringFormat(os, msg, targetCoordinate.GetLayerIndex(), targetCoordinate.GetElementIndex());
            }
            else
            {
                auto msg = "    // reassigning temporary variable to coordinate (%i,%i)\n";
                utilities::StringFormat(os, msg, targetCoordinate.GetLayerIndex(), targetCoordinate.GetElementIndex());
            }

            // assign name from int stack
            targetNode.SetTempVariableIndex(stack.Pop());
        }
        else
        {
            os << "    ";
        }

    }

    void ProcessNode(DataFlowNode& currentNode, DataFlowGraph& graph, utilities::IntegerStack& stack, std::ostream& os)
    {
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
                ProcessNode(targetNode, graph, stack, os);
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

    // print comment
    auto str = "// Predict function\n// Input dimension: %i\n// Output dimension: %i\n// Output coordinates:";
    uint64 inputLayerSize = _layers[0]->Size();
    utilities::StringFormat(os, str, inputLayerSize, outputLayerSize);
    for (uint64 i = 0; i < coordinateList.size(); ++i)
    {
        os << ' ' << coordinateList[i];
    }

    // print function declaration
    os << "\nvoid Predict(const double* input, double* output)\n{\n";

    // forwards pass to generate code
    const std::string inputNamePrefix = "input";
    for (uint64 inputElementIndex = 0; inputElementIndex < inputLayerSize; ++inputElementIndex)
    {
        layers::Coordinate inputCoordinate(0, inputElementIndex);
        auto& inputNode = graph.GetNode(inputCoordinate);

        std::string inputVariableName = inputNamePrefix + "[" + std::to_string(inputElementIndex) + "]";

        // if input has multiple actions, first copy it to a temp variable. Otherwise, operate directly on the input array
        if (inputNode.GetActions().size() <= 1)
        {
            inputNode.SetFixedVariableName(inputVariableName);
        }
        else
        {
            if (stack.IsTopNovel())
            {
                auto msg = "    // allocating temporary variable for coordinate (0,%i)\n    double "; // TODO unify this with similar code above
                utilities::StringFormat(os, msg, inputElementIndex);                                  // TODO put comment after code, not above
            }
            else
            {
                auto msg = "    // reassigning temporary variable to coordinate (0,%i)\n    ";
                utilities::StringFormat(os, msg, inputElementIndex);
            }

            inputNode.SetTempVariableIndex(stack.Pop());
            os << inputNode.GetVariableName() << " = " << inputVariableName << ";\n";
        }

        ProcessNode(inputNode, graph, stack, os);
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
