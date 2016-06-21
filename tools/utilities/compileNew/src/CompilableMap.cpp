////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompilableMap.cpp (compile)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableMap.h"
#include "CompilableCoordinatewise.h"
#include "CompilableSum.h"
#include "DataFlowNode.h"
#include "CodeEmitter.h"

// utilities
#include "IntegerStack.h"
#include "Format.h"

// stl
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>


void ProcessNode(DataFlowNode& currentNode, emll::compiler::CodeEmitter& codeGen)
{
    // for each action
    while(currentNode.HasActions())
    {
        // get the next action in the current node
        auto action = currentNode.PopAction();

		auto targetCoordinate = action.GetTarget();

		auto& targetNode = codeGen.LinearOp(action.GetOperation(), currentNode, action.GetTarget());

        // indicate that the target node is initialized
        targetNode.SetInitialized();

        // check if temp variable can be released
        if(currentNode.HasActions() == false && currentNode.HasTempVariableName())
        {
			codeGen.ReleaseVar(currentNode);
        }

        // if target node has all of its inputs, process it 
        targetNode.DecrementUncomputedInputs();
        if(targetNode.IsWaitingForInputs() == false)
        {
			ProcessNode(targetNode, codeGen);
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
	emll::compiler::CEmitter codeGen(graph, os);
	//emll::compiler::IREmitter codeGen(graph, os);

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

		auto& outputNode = graph.GetNode(outputCoordinate);
        outputNode.SetFixedVariableName(outputFixedVariableName, outputElementIndex);
        graph.GetNode(inputCoordinate).EmplaceAction(outputCoordinate);
		outputNode.IncrementUncomputedInputs();
    }

    // backwards pass to assign actions to nodes
    for(uint64_t layerIndex = outputLayerIndex-1; layerIndex > 0; --layerIndex)
    {
        _compilableLayers[layerIndex-1]->SetActions(layerIndex, graph);
    }


    // forward pass to generate code
    const std::string inputNamePrefix = "input";

	codeGen.Begin();
	codeGen.BeginLinear("Predict", inputNamePrefix, _requiredInputLayerSize, outputFixedVariableName, _outputCoordinates);

	for (uint64_t inputElementIndex = 0; inputElementIndex < _requiredInputLayerSize; ++inputElementIndex)
    {
        layers::Coordinate inputCoordinate(0, inputElementIndex);
        auto& inputNode = graph.GetNode(inputCoordinate);

        // if input has multiple actions, first copy it to a temp variable. Otherwise, operate directly on the input array
        if (inputNode.GetActions().size() <= 1)
        {
            inputNode.SetFixedVariableName(inputNamePrefix, inputElementIndex);
        }
        else
        {
			ScalarVariable var(inputNamePrefix, inputElementIndex);
			codeGen.Assign(var, inputNode, inputCoordinate);
        }

		ProcessNode(inputNode, codeGen);
    }

  codeGen.EndLinear();
  //codeGen.EmitTest("Predict", 124, 1, 5);
  codeGen.End();
}
