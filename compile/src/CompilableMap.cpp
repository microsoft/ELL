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
    void ProcessNode(const layers::Coordinate& currentCoordinate, DataFlowGraph& graph, std::ostream& os) 
    {
        auto& currentNode = graph[currentCoordinate.GetLayerIndex()][currentCoordinate.GetElementIndex()];
        const auto& currentVariableName = currentNode.VariableName;
        assert(currentVariableName != "");

        // for each action
        while (!currentNode.Actions.empty())
        {
            const auto& action = currentNode.Actions.back();

            // get target
            
            // check if it has a name, if not: get a temp name, set the name in the node, check if the name is new and write "double", print the name
            
            // check if it is initialized: yes then pring "+=", no then use "="

            // get the operaiton and print it with currentVariableName

            // remove the action
            currentNode.Actions.pop_back();

            // if empty and temporary, release the temp name

            // subtract 1 from target counter; if reached zero, call ProcessNode

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
    const std::string inputVariableName = "input";
    for (uint64 elementIndex = 0; elementIndex < inputLayerSize; ++elementIndex)
    {
        graph[0][elementIndex].VariableName = inputVariableName + "[" + std::to_string(elementIndex) + "]";
    }

    // add extra layer for outputs
    uint64 outputLayerSize = coordinateList.size();
    uint64 outputLayerIndex = NumLayers();
    graph.AddLayer(outputLayerSize);

    // set names on output layer
    const std::string outputVariableName = "output";
    for (uint64 elementIndex = 0; elementIndex < outputLayerSize; ++elementIndex)
    {
        graph[outputLayerIndex][elementIndex].VariableName = outputVariableName + "[" + std::to_string(elementIndex) + "]";
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
    utilities::IntegerStack integerStack;

    // forwards pass, to generate code
    for (uint64 inputElementIndex = 0; inputElementIndex < inputLayerSize; ++inputElementIndex)
    {
        layers::Coordinate inputCoordinate(0, inputElementIndex);
        ProcessNode(inputCoordinate, graph, os);
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
