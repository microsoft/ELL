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

// stl
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>
namespace
{
    // TODO signature needs output stream and intgereStack
    void ProcessNode(const layers::Coordinate& currentCoordinate, DataFlowGraph& graph, utilities::IntegerStack& stack, std::ostream& os)
    {
        auto& currentNode = graph[currentCoordinate.GetLayerIndex()][currentCoordinate.GetElementIndex()];
        auto currentNodeVariableName = currentNode.GetVariableName();

        // for each action
        while(!currentNode.Actions.empty())
        {
            // get the next action in the current node
            const auto& action = currentNode.Actions.back();

            // get the target coordinate and node
            auto targetCoordinate = action.GetTarget();
            auto& targetNode = graph[targetCoordinate.GetLayerIndex()][targetCoordinate.GetElementIndex()];

            if(!targetNode.HasVariableName())
            {
                if(stack.IsTopNovel())
                {
                    os << "double ";
                }

                // assign name from int stack
                targetNode.SetTempVariableIndex(stack.Pop());
            }

            // print variable name
            os << targetNode.GetVariableName() << ' ';

            // print either '=' or '+='
            if(targetNode.IsInitialized)
            {
                os << "+= ";
            }
            else
            {
                os << "= ";
                targetNode.IsInitialized = true;
            }

            // print the right hand side
            action.GetOperation().Print(currentNodeVariableName, os);

            // remove the action
            currentNode.Actions.pop_back();

            if(currentNode.Actions.empty())
            {
                // release temp variable
                stack.Push(currentNode.GetTempVariableIndex());
            }

            --(targetNode.NumUncomputedInputs);

            if(targetNode.NumUncomputedInputs == 0)
            {
                ProcessNode(targetCoordinate, graph, stack, os);
            }
        }
    }
}

void CompilableMap::ToCode(layers::CoordinateList coordinateList, std::ostream& os) const
{
    // allocate datastructure to hold actions
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
        graph[0][elementIndex].FixedVariableName = inputFixedVariableName + "[" + std::to_string(elementIndex) + "]";
    }

    // add extra layer for outputs
    uint64 outputLayerSize = coordinateList.size();
    uint64 outputLayerIndex = NumLayers();
    graph.AddLayer(outputLayerSize);

    // set names on output layer
    const std::string outputFixedVariableName = "output";
    for (uint64 elementIndex = 0; elementIndex < outputLayerSize; ++elementIndex)
    {
        graph[outputLayerIndex][elementIndex].FixedVariableName = outputFixedVariableName + "[" + std::to_string(elementIndex) + "]";
    }

    // set the actions that generate the output coordinates
    for (uint64 outputElementIndex = 0; outputElementIndex < outputLayerSize; ++outputElementIndex)
    {
        auto inputCoordinate = coordinateList[outputElementIndex];
        layers::Coordinate outputCoordinate(outputLayerIndex, outputElementIndex);
        graph[inputCoordinate.GetLayerIndex()][inputCoordinate.GetElementIndex()].Actions.emplace_back(outputCoordinate);
    }

    // backwards pass to assign actions
    for(uint64 layerIndex = NumLayers() - 1; layerIndex > 0; --layerIndex)
    {
        auto compilableLayer = GetLayer<CompilableLayer>(layerIndex);
        compilableLayer->SetActions(layerIndex, graph);
    }

    // allocate integer stack, to manage temp variable names;
    utilities::IntegerStack stack;

    // forwards pass, to generate code
    for (uint64 inputElementIndex = 0; inputElementIndex < inputLayerSize; ++inputElementIndex)
    {
        layers::Coordinate inputCoordinate(0, inputElementIndex);
        ProcessNode(inputCoordinate, graph, stack, os);
    }
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
