////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     CompilableMap.cpp (compile)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableMap.h"
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
    _outputCoordinates = other.GetOutputCoordinateList();

    const auto& model = other.GetModel();
    _requiredInputLayerSize = std::max(model.GetRequiredLayerSize(0), _outputCoordinates.GetRequiredLayerSize(0));

    utilities::TypeFactory<CompilableLayer> compilableLayerFactory;
    compilableLayerFactory.AddType<CompilableCoordinatewise>(layers::Coordinatewise::GetTypeName());
    compilableLayerFactory.AddType<CompilableSum>(layers::Sum::GetTypeName());

    // input layer is stored implicitly, copy all other layers
    for (uint64_t index = 1; index < model.NumLayers(); ++index)
    {
        const auto& layer = model.GetLayer(index);
        _compilableLayers.push_back(compilableLayerFactory.Construct(layer.GetRuntimeTypeName()));
        (*_compilableLayers.back()) = layer;
    }
}

void CompilableMap::ToCode(std::ostream& os) const
{
    uint64_t numLayersExcludingInput = _compilableLayers.size();

    // create data flow graph datastructure
    DataFlowGraph graph;

    // add graph layer for input
    graph.AddLayer(_requiredInputLayerSize);

    // add graph later for other layers
    for(uint64_t i = 0; i < numLayersExcludingInput; ++i)
    {
        graph.AddLayer(_compilableLayers[i]->GetOutputDimension());
    }

    // add an extra layer for outputs
    uint64_t outputLayerSize = _outputCoordinates.Size();
    uint64_t outputLayerIndex = numLayersExcludingInput+1;
    graph.AddLayer(outputLayerSize);

    // set names on output layer and set the actions that generate the output coordinates
    const std::string outputFixedVariableName = "output";
    for (uint64_t outputElementIndex = 0; outputElementIndex < outputLayerSize; ++outputElementIndex)
    {
        auto inputCoordinate = _outputCoordinates[outputElementIndex];
        layers::Coordinate outputCoordinate(outputLayerIndex, outputElementIndex);
        graph.GetNode(outputCoordinate).SetFixedVariableName(outputFixedVariableName + "[" + std::to_string(outputElementIndex) + "]");
        graph.GetNode(inputCoordinate).EmplaceAction(outputCoordinate);
    }

    // backwards pass to assign actions to nodes
    for(uint64_t layerIndex = outputLayerIndex-1; layerIndex > 0; --layerIndex)
    {
        _compilableLayers[layerIndex-1]->SetActions(layerIndex, graph);
    }

    // construct an integer stack, to manage temp variable names;
    utilities::IntegerStack stack;

    // print comment
    auto str = "// Predict function\n// Input dimension: %\n// Output dimension: %\n// Output coordinates:";
    utilities::PrintFormat(os, str, _requiredInputLayerSize, outputLayerSize);
    for (uint64_t i = 0; i < _outputCoordinates.Size(); ++i)
    {
        os << ' ' << _outputCoordinates[i];
    }

    // print function declaration
    os << "\nvoid Predict(const double* input, double* output)\n{\n";

    // forward pass to generate code
    const std::string inputNamePrefix = "input";
    for (uint64_t inputElementIndex = 0; inputElementIndex < _requiredInputLayerSize; ++inputElementIndex)
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
