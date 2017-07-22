////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelComparison.h (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DgmlGraph.h"
#include "IRExecutionEngine.h"
#include "ResizeImage.h"

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
#include <vector>

using namespace ell;

class ModelComparison
{
public:
    ModelComparison(std::string outputDirectory);

    static ModelComparison* GetInstance() { return _instance; }

    void SetupReferenceMap(model::DynamicMap& map);
    
    void Compare(std::vector<float>& input, model::DynamicMap& reference);

    void SaveOutput(std::string name, std::vector<float> reference, std::vector<float> compiled);

    size_t GetOutputSize(const std::string& id);

    void WriteReport(std::ostream& outputStream);

    void AddStyles();

    void AddLayer(const char* label, const float* output);

    void CreateGraph(const model::Model& model);

    void SaveGraph(std::ostream& stm);

    void AddDebugOutputNode(model::ModelTransformer& transformer, const nodes::NeuralNetworkLayerNodeBase<float>* layerNode);

private:

    struct LayerCaptureData
    {
    public:
        const ell::nodes::DebugSinkNode<float>* referenceDebugNode;
        const ell::nodes::DebugSinkNode<float>* compiledDebugNode;
        std::vector<size_t> size;
        std::vector<size_t> stride;
        std::vector<size_t> offset;
        std::string ReferenceNodeLabel;
        std::string CompiledNodeId;
        std::string CompiledNodeLabel;
        std::vector<float> Reference;
        std::vector<float> Compiled;
    };

    void WriteRow(std::ostream& outputStream, std::string id, std::string name, std::vector<float>& reference, std::vector<float>& compiled, LayerCaptureData* c);

    static ModelComparison* _instance;
    bool _addingReference;
    DgmlGraph _graph;
    const model::Model* model;
    std::map<std::string, std::string> _nodeMap; // map id of reference node to compiled node.
    std::map<std::string, size_t> _outputSizes;
    model::DynamicMap _referenceMap;
    std::vector<float> _outputReference;
    std::vector<float> _outputCompiled;
    std::string _outputDirectory;
    bool _runningCompiled;
    std::vector<LayerCaptureData*> _layers;
    int _nextIndex;
    double _minError;
    double _maxError;
    bool _hasMinMax;
};
