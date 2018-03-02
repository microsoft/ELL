////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelComparison.h (debugCompiler)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Graph.h"
#include "VectorStatistics.h"

// model
#include "DebugSinkNode.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "InputPort.h"
#include "LoadModel.h"
#include "Map.h"
#include "Model.h"
#include "ModelLoadArguments.h"
#include "NeuralNetworkLayerNode.h"
#include "OutputNode.h"

// stl
#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace ell
{
class ModelComparison
{
public:
    /// <summary> Constructor </summary>
    ///
    /// <param name="outputDirectory"> The directory the output files will be written to. </param>
    ModelComparison(std::string outputDirectory);

    /// <summary> Compares the "reference" output vs. compiled otuput of a map. </summary>
    ///
    /// <param name="input"> The data to evaluate the map with when comparing outputs. </param>
    /// <param name="reference"> The map to compare the reference vs. compiled versions of. </param>
    /// <param name="settings"> The compiler settings to use when compiling the map. </param>
    void Compare(std::vector<float>& input, model::Map& reference, const model::MapCompilerOptions& settings);

    /// <summary> Saves reference and compiled outputs to a file. </summary>
    ///
    /// <param name="name"> The label for the output from with the filename is generated. </param>
    /// <param name="reference"> The output of the reference implementation. </param>
    /// <param name="compiled"> The output of the compiled implementation. </param>
    void SaveOutput(std::string name, const std::vector<float>& reference, const std::vector<float>& compiled);

    /// <summary> Gets the output size of a node, given the node's ID. </summary>
    ///
    /// <param name="nodeId"> The ID of the node. </param>
    /// <returns> The size of the node. </returns>
    size_t GetOutputSize(const std::string& nodeId);

    /// <summary> Writes a report with summary and detail of differences between reference and compiled outputs. </summary>
    ///
    /// <param name="outputStream"> The stream to write output to. </param>
    /// <param name="modelName"> The name of the model. </param>
    /// <param name="testArgs"> The arguments used during evaluation. </param>
    /// <param name="writePrediction"> Indicates whether to write the predicted output (== argmax of the output vector) of the model. </param>
    void WriteReport(std::ostream& outputStream, std::string modelName, const std::vector<std::string>& testArgs, bool writePrediction);

    /// <summary> Writes a DGML file of the model, annotated with differences between reference and compiled outputs. </summary>
    ///
    /// <param name="outputStream"> The stream to write output to. </param>
    void SaveDgml(std::ostream& outputStream);

    /// <summary> Writes a DOT graph file of the model, annotated with differences between reference and compiled outputs. </summary>
    ///
    /// <param name="outputStream"> The stream to write output to. </param>
    void SaveDot(std::ostream& outputStream);

    /// <summary> Records a layer's output to the internal list of outputs. Used internally, but public because it needs to be called from a callback routine. </summary>
    ///
    /// <param name="label"> The label for the layer. </param>
    /// <param name="output"> The output data for the layer. </param>
    template <typename ValueType>
    void AddLayer(const char* label, const ValueType* output);

private:
    struct LayerCaptureData
    {
        const ell::model::Node* referenceDebugNode; // sink node
        const ell::model::Node* compiledDebugNode; // sink node
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

    void SetUpReferenceMap(model::Map& map);
    void CreateGraph(const model::Model& model);
    void AddStyles();
    void WriteModelInfo(std::ostream& outputStream, const std::vector<float>& reference, const std::vector<float>& compiled, bool writePrediction);
    void WriteNodeRow(std::ostream& outputStream, std::string id, std::string name, const std::vector<float>& reference, const std::vector<float>& compiled, const LayerCaptureData& layerData);
    void WriteNodeDetail(std::ostream& outputStream, const model::Node* node);
    void WriteStatsRow(std::ostream& outputStream, const VectorStatistics& refStats, const VectorStatistics& compiledStats, const VectorStatistics& diffStats, float sumAbsDiff);
    template <typename ValueType>
    void WriteLayerNodeDetail(std::ostream& outputStream, const ell::nodes::NeuralNetworkLayerNodeBase<ValueType>* layerNode);

    bool _addingReference;
    ell::utilities::Graph _graph;
    std::map<std::string, std::string> _nodeMap; // map id of reference node to compiled node.
    std::map<std::string, size_t> _outputSizes;
    model::Map _referenceMap;
    std::vector<float> _outputReference;
    std::vector<float> _outputCompiled;
    std::string _outputDirectory;
    bool _runningCompiled;
    std::vector<LayerCaptureData> _layerOutputData;
    size_t _nextIndex;
    double _minError;
    double _maxError;
    bool _hasMinMax;
};
}
