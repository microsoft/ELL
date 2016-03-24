////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     CompilableMap.cpp (compile)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableMap.h"
#include "CompilableInput.h"
#include "CompilableCoordinatewise.h"
#include "CompilableSum.h"
#include "DataFlowNode.h"

// utilities
#include "IntegerStack.h"
#include "Format.h"

// stl
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

const char* assignmentFormat = "    % = %; // coordinate (%,%)\n";
const char* incrementFormat = "    % += %; // coordinate (%,%)\n";
const char* allocationFormat = "    double % = %; // coordinate (%,%), allocating new temporary variable\n";
const char* reallocationFormat = "    % = %; // coordinate (%,%), reassigning temporary variable\n";

const char* AllocateTempVariableAndGetFormat(DataFlowNode& targetNode, utilities::IntegerStack& stack)
{
    if (targetNode.IsInitialized() == true)
    {
        return incrementFormat;
    }

    if (targetNode.HasFixedVariableName() == true)
    {
        return assignmentFormat;
    }

    bool isNovel = stack.IsTopNovel();
    targetNode.SetTempVariableIndex(stack.Pop());

    if (isNovel)
    {
        return allocationFormat;
    }
    else
    {
        return reallocationFormat;
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

        const char* format = AllocateTempVariableAndGetFormat(targetNode, stack);
        auto targetVariableName = targetNode.GetVariableName();
        auto rhs = action.GetOperation().ToString(currentNodeVariableName);

        utilities::PrintFormat(os, format, targetVariableName, rhs, targetCoordinate.GetLayerIndex(), targetCoordinate.GetElementIndex());

        // indicate that the target node is initialized
        targetNode.SetInitialized();

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

CompilableMap::CompilableMap(const layers::Map& other)
{
    _outputCoordinates = other.GetOutputCoordinates();

    const auto& stack = other.GetStack();
    utilities::TypeFactory<CompilableLayer> compilableLayerFactory;
    compilableLayerFactory.AddType<CompilableInput>(layers::Input::GetTypeName());
    compilableLayerFactory.AddType<CompilableCoordinatewise>(layers::Coordinatewise::GetTypeName());
    compilableLayerFactory.AddType<CompilableSum>(layers::Sum::GetTypeName());

    for (uint64 index = 0; index < stack.NumLayers(); ++index)
    {
        const auto& layer = stack.GetLayer(index);
        _compilableLayers.push_back(compilableLayerFactory.Construct(layer.GetRuntimeTypeName()));
        (*_compilableLayers.back()) = layer;
    }
}

void CompilableMap::ToCode(std::ostream& os) const
{
    uint64 numLayers = _compilableLayers.size();
    if (numLayers == 0)
    {
        throw std::runtime_error("Error: calling ToCode on an empty CompilableMap");
    }

    // create data flow graph datastructure
    DataFlowGraph graph;
    for(uint64 layerIndex = 0; layerIndex < numLayers; ++layerIndex)
    {
        graph.AddLayer(_compilableLayers[layerIndex]->Size());
    }

    // add extra layer for outputs
    uint64 outputLayerSize = _outputCoordinates.size();
    uint64 outputLayerIndex = numLayers;
    graph.AddLayer(outputLayerSize);

    // set names on output layer and set the actions that generate the output coordinates
    const std::string outputFixedVariableName = "output";
    for (uint64 outputElementIndex = 0; outputElementIndex < outputLayerSize; ++outputElementIndex)
    {
        auto inputCoordinate = _outputCoordinates[outputElementIndex];
        layers::Coordinate outputCoordinate(outputLayerIndex, outputElementIndex);
        graph.GetNode(outputCoordinate).SetFixedVariableName(outputFixedVariableName + "[" + std::to_string(outputElementIndex) + "]");
        graph.GetNode(inputCoordinate).EmplaceAction(outputCoordinate);
    }

    // backwards pass to assign actions to nodes
    for(uint64 layerIndex = numLayers - 1; layerIndex > 0; --layerIndex)
    {
        _compilableLayers[layerIndex]->SetActions(layerIndex, graph);
    }

    // construct an integer stack, to manage temp variable names;
    utilities::IntegerStack stack;

    // print comment
    auto str = "// Predict function\n// Input dimension: %i\n// Output dimension: %i\n// Output coordinates:";
    uint64 inputLayerSize = _compilableLayers[0]->Size();
    utilities::PrintFormat(os, str, inputLayerSize, outputLayerSize);
    for (uint64 i = 0; i < _outputCoordinates.size(); ++i)
    {
        os << ' ' << _outputCoordinates[i];
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
            const char* format = AllocateTempVariableAndGetFormat(inputNode, stack);
            utilities::PrintFormat(os, format, inputNode.GetVariableName(), inputVariableName, 0, inputElementIndex);
        }
        ProcessNode(inputNode, graph, stack, os);
    }

    os << "}\n";
}
