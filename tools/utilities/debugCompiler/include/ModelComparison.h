////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelComparison.h (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Graph.h"
#include "IRExecutionEngine.h"

// model
#include "DebugSinkNode.h"
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "LoadModel.h"
#include "Model.h"
#include "ModelLoadArguments.h"
#include "NeuralNetworkLayerNode.h"
#include "OutputNode.h"

// stl
#include <iostream>
#include <map>
#include <memory>
#include <vector>

using namespace ell;

class ModelComparison
{
public:
    ModelComparison(std::string outputDirectory);

    void Compare(std::vector<float>& input, model::DynamicMap& reference, bool useBlas, bool optimize, bool allowVectorInstructions, bool fuseLinearOps);

    void SaveOutput(std::string name, const std::vector<float>& reference, const std::vector<float>& compiled);

    size_t GetOutputSize(const std::string& nodeId);

    void WriteReport(std::ostream& outputStream, std::string modelName, std::string testDataName);

    void SaveDgml(std::ostream& stm);

    void SaveDot(std::ostream& stm);

    template <typename ValueType>
    void AddLayer(const char* label, const ValueType* output);

private:
    struct LayerCaptureData
    {
        const ell::model::Node* referenceDebugNode;
        const ell::model::Node* compiledDebugNode;
        std::string referenceNodeLabel;
        std::string compiledNodeId;
        std::string compiledNodeLabel;
        std::vector<int> size;
        std::vector<int> stride;
        std::vector<int> offset;
        std::vector<float> referenceData;
        std::vector<float> compiledData;
    };

    void AddDebugOutputNode(model::ModelTransformer& transformer, const model::Node& node);
    template <typename ValueType>
    void AddDebugOutputNode(model::ModelTransformer& transformer, const nodes::NeuralNetworkLayerNodeBase<ValueType>* layerNode);

    void SetUpReferenceMap(model::DynamicMap& map);
    void CreateGraph(const model::Model& model);
    void AddStyles();
    void WriteRow(std::ostream& outputStream, std::string id, std::string name, const std::vector<float>& reference, const std::vector<float>& compiled, LayerCaptureData* layerData);
    void WriteNodeDetail(std::ostream& outputStream, const model::Node* node);
    template <typename ValueType>
    void WriteLayerNodeDetail(std::ostream& outputStream, const ell::nodes::NeuralNetworkLayerNodeBase<ValueType>* layerNode);
    
    bool _addingReference;
    ell::utilities::Graph _graph;
    std::map<std::string, std::string> _nodeMap; // map id of reference node to compiled node.
    std::map<std::string, size_t> _outputSizes;
    model::DynamicMap _referenceMap;
    std::vector<float> _outputReference;
    std::vector<float> _outputCompiled;
    std::string _outputDirectory;
    bool _runningCompiled;
    std::vector<LayerCaptureData> _layerOutputData;
    int _nextIndex;
    double _minError;
    double _maxError;
    bool _hasMinMax;
};
