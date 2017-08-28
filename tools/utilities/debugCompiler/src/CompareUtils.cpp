//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompareUtils.cpp (debugNet)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompareUtils.h"

// common
#include "LoadModel.h"
#include "MapLoadArguments.h"

// model
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "Model.h"
#include "Port.h"

// nodes
#include "DebugSinkNode.h"
#include "NeuralNetworkPredictorNode.h"

// predictors
#include "Layer.h"
#include "NeuralNetworkPredictor.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "MillisecondTimer.h"
#include "PPMImageParser.h"

// stl
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>

using namespace ell;

//
// General
//
void PrintPortElements(std::ostream& os, const model::PortElementsBase& elements)
{
    if (elements.NumRanges() > 1)
    {
        os << "{";
    }

    bool isFirstRange = true;
    for (const auto& range : elements.GetRanges())
    {
        os << (isFirstRange ? "" : ", ");
        isFirstRange = false;

        auto port = range.ReferencedPort();
        os << "node_" << port->GetNode()->GetId() << "." << port->GetName();
        if (!range.IsFullPortRange())
        {
            auto start = range.GetStartIndex();
            auto size = range.Size();
            os << "[" << start << ":" << (start + size) << "]";
        }
    }

    if (elements.NumRanges() > 1)
    {
        os << "}";
    }
}

//
// Sink-node-related
//
std::string GetSinkNodeLabel(const ell::model::Node* node)
{
    auto floatSinkNode = dynamic_cast<const ell::nodes::DebugSinkNode<float>*>(node);
    if (floatSinkNode != nullptr)
    {
        return floatSinkNode->GetLabel();
    }

    auto doubleSinkNode = dynamic_cast<const ell::nodes::DebugSinkNode<double>*>(node);
    if (doubleSinkNode != nullptr)
    {
        return doubleSinkNode->GetLabel();
    }

    return "";
}

//
// Neural-net-related
//
bool IsNeuralNetworkPredictorNode(const ell::model::Node* node)
{
    if (dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>*>(node) != nullptr)
    {
        return true;
    }

    return dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<double>*>(node) != nullptr;
}

bool IsNeuralNetworkLayerNode(const ell::model::Node* node)
{
    if (dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<float>*>(node) != nullptr)
    {
        return true;
    }

    return dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<double>*>(node) != nullptr;
}

bool HasNeuralNetworkPredictor(const ell::model::DynamicMap& map)
{
    return HasNeuralNetworkPredictor<float>(map) || HasNeuralNetworkPredictor<double>(map);
}

template <>
bool HasNeuralNetworkPredictor<int>(const ell::model::DynamicMap& map)
{
    return false;
}

template <>
bool HasNeuralNetworkPredictor<int64_t>(const ell::model::DynamicMap& map)
{
    return false;
}

template <>
ell::predictors::NeuralNetworkPredictor<float> GetNeuralNetworkPredictor(ell::model::DynamicMap& map)
{
    auto& model = map.GetModel();
    auto nodeIter = model.GetNodeIterator();
    while (nodeIter.IsValid())
    {
        auto node = nodeIter.Get();
        auto nnNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>*>(node);
        if (nnNode != nullptr)
        {
            return nnNode->GetPredictor();
        }
        nodeIter.Next();
    }
    throw ell::utilities::InputException(ell::utilities::InputExceptionErrors::invalidArgument, "Model must contain a neural network predictor");
}

template <>
ell::predictors::NeuralNetworkPredictor<double> GetNeuralNetworkPredictor(ell::model::DynamicMap& map)
{
    auto& model = map.GetModel();
    auto nodeIter = model.GetNodeIterator();
    while (nodeIter.IsValid())
    {
        auto node = nodeIter.Get();
        auto nnNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<double>*>(node);
        if (nnNode != nullptr)
        {
            return nnNode->GetPredictor();
        }
        nodeIter.Next();
    }
    throw ell::utilities::InputException(ell::utilities::InputExceptionErrors::invalidArgument, "Model must contain a neural network predictor");
}

Shape GetInputShape(const ell::model::DynamicMap& map)
{
    auto inputSize = map.GetInputSize();

    // Guess if this takes an image as input. If not return a shape of (N,1,1)
    // TODO: add a way for maps / models to annotate their input with shape info
    if (HasNeuralNetworkPredictor(map))
    {
        // Guess the size of the input image
        size_t numChannels = 3;
        size_t numPixels = inputSize / 3;
        size_t width = static_cast<size_t>(std::sqrt(numPixels));
        if (inputSize % numChannels || width * width != numPixels)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model must consume a square image with 3 channels");
        }
        size_t height = width;
        return { height, width, numChannels };
    }
    else
    {
        return { inputSize, 1, 1 };
    }
}

std::string SanitizeTypeName(std::string layerType)
{
    std::replace(layerType.begin(), layerType.end(), '<', '_');
    std::replace(layerType.begin(), layerType.end(), '>', '_');
    std::replace(layerType.begin(), layerType.end(), ',', '_');
    return layerType;
}

std::string GetLayerFilename(const std::string& layerType, int index)
{
    return std::string{ "Layer_" } + std::to_string(index) + "_" + SanitizeTypeName(layerType) + ".tsv";
}

std::string GetNodeOutputFilename(const std::string& layerType, int index, const std::string& suffix)
{
    if (suffix == "")
        return std::string{ "Layer_" } + std::to_string(index) + "_" + SanitizeTypeName(layerType) + ".tsv";
    else
        return std::string{ "Layer_" } + std::to_string(index) + "_" + SanitizeTypeName(layerType) + "_" + suffix + ".tsv";
}

std::string GetNodeFilename(const std::string& nodeType, int index)
{
    return std::string{ "Node_" } + std::to_string(index) + "_" + SanitizeTypeName(nodeType) + ".tsv";
}
//
// Report-writing
//
const std::string css = R"xx(
        body {
padding: 0;
margin: 0;
font: 13px Arial, Helvetica, Garuda, sans-serif;
*font-size: small;
*font: x-small;
}
h1, h2, h3, h4, h5, h6, ul, li, em, strong, pre, code {
padding: 0;
margin: 0;
line-height: 1em;
font-size: 100%;
font-weight: normal;
font-style: normal;
}
table {
font-size: inherit;
font: 100%;
}
ul {
list-style: none;
}
img {
border: 0;
}
p {
margin: 1em 0;
}

body {
  padding: 8pt;
}

table {
  width: 100%;
  max-width: 100em;
}

h1 {
  font-size: 120%;
}

h2 {
  font-size: 110%;
  font-weight: bold;
}

th {
  text-align: left
}

em {
  font-weight: bold;
}

.layer {
  margin-bottom: 24pt;
  margin-left: 12pt;
}

.layer h2 {
  margin-left: -8pt;
}
)xx";

void WriteReportHeader(std::ostream& reportStream, const std::string& modelName)
{
    reportStream << "<!DOCTYPE html>\n";
    reportStream << "<html>\n";
    reportStream << "<head>\n";
    reportStream << "  <title> " << modelName << " </title>\n";
    reportStream << "  <style> " << css << "\n";
    reportStream << "  </style>\n";
    reportStream << "</head>\n";
    reportStream << "<body>\n";
}

void WriteModelReport(std::ostream& reportStream, const OutputInfo& layerOutput, const OutputInfo& nodeOutput)
{
    reportStream << "<div class='layer'>\n";
    reportStream << "<h2>Model summary</h2>\n";

    reportStream << "<table>\n";
    reportStream << "<tr>\n";
    reportStream << "<th></th>\n";
    if (layerOutput.outputs.size() > 0)
    {
        reportStream << "<th>" << layerOutput.name << "</th>\n";
    }
    if (nodeOutput.outputs.size() > 0)
    {
        reportStream << "<th>" << nodeOutput.name << "</th>\n";
    }
    if (layerOutput.outputs.size() > 0 && nodeOutput.outputs.size() > 0)
    {
        reportStream << "<th>difference</th>\n";
    }
    reportStream << "</tr>\n";

    VectorStats layerStats(layerOutput.outputs);
    VectorStats nodeStats(nodeOutput.outputs);

    std::vector<double> diffOutput;
    if (layerOutput.outputs.size() > 0 && nodeOutput.outputs.size() > 0)
    {
        const auto vecSize = layerOutput.outputs.size();
        assert(vecSize == nodeOutput.outputs.size());

        diffOutput.resize(vecSize);
        for (int index = 0; index < vecSize; ++index)
        {
            diffOutput[index] = layerOutput.outputs[index] - nodeOutput.outputs[index];
        }
    }
    VectorStats diffStats(diffOutput);

    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "min", [](const VectorStats& s) { return s.Min(); });
    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "max", [](const VectorStats& s) { return s.Max(); });
    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "mean", [](const VectorStats& s) { return s.Mean(); });
    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "stdDev", [](const VectorStats& s) { return s.StdDev(); });

    reportStream << "</table>\n";
    reportStream << "</div>\n";
    reportStream.flush();
}

template <typename ValueType>
void WriteLayerReport(std::ostream& reportStream, int index, const nodes::NeuralNetworkLayerNodeBase<ValueType>* layerNode, const OutputInfo& layerOutput, const OutputInfo& nodeOutput)
{
    auto layerType = layerNode->GetRuntimeTypeName();

    reportStream << "<div class='layer'>\n";
    reportStream << "<h2>Layer " << index << ": " << SanitizeTypeName(layerType) << "</h2>\n";
    auto inputLayout = layerNode->GetInputMemoryLayout();
    reportStream << "<p> <em>Input layout</em> ";
    reportStream << "size: " << inputLayout.size[0] << " x " << inputLayout.size[1] << " x " << inputLayout.size[2] << ", ";
    reportStream << "stride: " << inputLayout.stride[0] << " x " << inputLayout.stride[1] << " x " << inputLayout.stride[2] << ", ";
    reportStream << "offset: " << inputLayout.offset[0] << " x " << inputLayout.offset[1] << " x " << inputLayout.offset[2] << "</p>";

    auto outputLayout = layerNode->GetOutputMemoryLayout();
    reportStream << "<p> <em>Output layout</em> ";
    reportStream << "size: " << outputLayout.size[0] << " x " << outputLayout.size[1] << " x " << outputLayout.size[2] << ", ";
    reportStream << "stride: " << outputLayout.stride[0] << " x " << outputLayout.stride[1] << " x " << outputLayout.stride[2] << ", ";
    reportStream << "offset: " << outputLayout.offset[0] << " x " << outputLayout.offset[1] << " x " << outputLayout.offset[2] << "</p>";

    reportStream << "<table>\n";
    reportStream << "<tr>\n";
    reportStream << "<th></th>\n";
    if (layerOutput.outputs.size() > 0)
    {
        reportStream << "<th>" << layerOutput.name << "</th>\n";
    }
    if (nodeOutput.outputs.size() > 0)
    {
        reportStream << "<th>" << nodeOutput.name << "</th>\n";
    }
    if (layerOutput.outputs.size() > 0 && nodeOutput.outputs.size() > 0)
    {
        reportStream << "<th>difference</th>\n";
    }
    reportStream << "</tr>\n";

    VectorStats layerStats(layerOutput.outputs);
    VectorStats nodeStats(nodeOutput.outputs);

    std::vector<double> diffOutput;
    if (layerOutput.outputs.size() > 0 && nodeOutput.outputs.size() > 0)
    {
        const auto vecSize = layerOutput.outputs.size();
        assert(vecSize == nodeOutput.outputs.size());

        diffOutput.resize(vecSize);
        for (int index = 0; index < vecSize; ++index)
        {
            diffOutput[index] = layerOutput.outputs[index] - nodeOutput.outputs[index];
        }
    }
    VectorStats diffStats(diffOutput);

    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "min", [](const VectorStats& s) { return s.Min(); });
    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "max", [](const VectorStats& s) { return s.Max(); });
    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "mean", [](const VectorStats& s) { return s.Mean(); });
    WriteStatsRow(reportStream, layerStats, nodeStats, diffStats, "stdDev", [](const VectorStats& s) { return s.StdDev(); });

    reportStream << "</table>\n";
    reportStream << "</div>\n";
    reportStream.flush();
}

void WriteLayerReport(std::ostream& reportStream, int index, const model::Node* layerNode, const OutputInfo& layerOutput, const OutputInfo& nodeOutput)
{
    auto floatLayerNode = dynamic_cast<const nodes::NeuralNetworkLayerNodeBase<float>*>(layerNode);
    if (floatLayerNode != nullptr)
    {
        WriteLayerReport(reportStream, index, floatLayerNode, layerOutput, nodeOutput);
        return;
    }
    auto doubleLayerNode = dynamic_cast<const nodes::NeuralNetworkLayerNodeBase<double>*>(layerNode);
    if (doubleLayerNode != nullptr)
    {
        WriteLayerReport(reportStream, index, doubleLayerNode, layerOutput, nodeOutput);
        return;
    }
}

void WriteReportFooter(std::ostream& reportStream)
{
    reportStream << "</body>\n";
    reportStream << "</html>\n";
}

//
// generating test data
//
double GetPixelVal(double x, double y, int ch, int index)
{
    switch (index)
    {
        case 0:
            return std::sin(x) + std::cos(y * 2 + ch);

        case 1:
            return std::sin(x * 1.5) + std::cos(y * 2 + ch);

        case 2:
            return std::sin(x) + std::cos(y * 2.5 + ch);

        case 3:
            return std::sin(x) + std::cos(y * .25 + ch);

        default:
            return std::sin(x * 0.25 + ch / 8.0) + std::cos(y * 2 + ch);
    }
}
