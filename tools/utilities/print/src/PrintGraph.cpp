////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintGraph.cpp (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LayerInspector.h"
#include "PrintModel.h"

#include <utilities/include/Exception.h>
#include <utilities/include/Graph.h>
#include <utilities/include/OutputStreamImpostor.h>
#include <utilities/include/StringUtil.h>

#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/Node.h>

#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/BroadcastOperationNodes.h>
#include <nodes/include/NeuralNetworkPredictorNode.h>
#include <nodes/include/UnaryOperationNode.h>

#include <iostream>
#include <string>

using namespace ell::utilities;
using namespace ell::model;
using namespace ell::nodes;

namespace ell
{
extern std::string PaddingSchemeToString(ell::predictors::neural::PaddingScheme scheme);

std::string PrintActiveSize(const Port* port)
{
    std::string shape;
    if (port != nullptr)
    {
        auto layout = port->GetMemoryLayout();
        auto memoryShape = layout.GetActiveSize().ToVector();
        for (int i : memoryShape)
        {
            if (shape.size() > 0)
            {
                shape += " &#xd7; "; // multiplication symbol
            }
            shape += std::to_string(i);
        }
    }
    return shape;
}

std::string ToShortString(BinaryOperationType op)
{
    switch (op)
    {
    case BinaryOperationType::none:
        return "none";
    case BinaryOperationType::add:
        return "+";
    case BinaryOperationType::subtract:
        return "-";
    case BinaryOperationType::multiply:
        return "*";
    case BinaryOperationType::divide:
        return "/";
    case BinaryOperationType::logicalAnd:
        return "&&";
    case BinaryOperationType::logicalOr:
        return "||";
    case BinaryOperationType::logicalXor:
        return "^";
    }
    return "";
}

template <typename ValueType>
std::string GetBroadcastBinaryOperationType(const Node& node)
{
    auto opnode = dynamic_cast<const BroadcastBinaryOperationNode<ValueType>*>(&node);
    return ToShortString(opnode->GetOperation());
}

template <typename ValueType>
std::string GetBinaryOperationType(const Node& node)
{
    auto opnode = dynamic_cast<const BinaryOperationNode<ValueType>*>(&node);
    return ToShortString(opnode->GetOperation());
}

template <typename ValueType>
std::string GetUnaryOperationType(const Node& node)
{
    auto opnode = dynamic_cast<const UnaryOperationNode<ValueType>*>(&node);
    return ToString(opnode->GetOperation());
}

std::string GetNodeLabel(const Node& node, std::vector<NameValue>& properties)
{
    std::string typeName = node.GetRuntimeTypeName();
    std::vector<std::string> parts = Split(typeName, '<');
    typeName = parts[0];
    if (parts.size() > 1)
    {
        if (typeName == "BinaryOperationNode")
        {
            std::string opName;
            if (parts[1] == "float>")
            {
                opName = GetBinaryOperationType<float>(node);
            }
            else if (parts[1] == "double>")
            {
                opName = GetBinaryOperationType<float>(node);
            }
            if (opName.size() > 0)
            {
                properties.push_back(NameValue{ "name", typeName });
                typeName = opName;
            }
        }
        else if (typeName == "BroadcastBinaryOperationNode")
        {
            std::string opName;
            if (parts[1] == "float>")
            {
                opName = GetBroadcastBinaryOperationType<float>(node);
            }
            else if (parts[1] == "double>")
            {
                opName = GetBroadcastBinaryOperationType<float>(node);
            }
            if (opName.size() > 0)
            {
                properties.push_back(NameValue{ "name", typeName });
                typeName = opName;
            }
        }
        else if (typeName == "UnaryOperationNode")
        {
            std::string opName;
            if (parts[1] == "float>")
            {
                opName = GetUnaryOperationType<float>(node);
            }
            else if (parts[1] == "double>")
            {
                opName = GetUnaryOperationType<float>(node);
            }
            if (opName.size() > 0)
            {
                properties.push_back(NameValue{ "name", typeName });
                typeName = opName;
            }
        }
    }
    if (node.GetMetadata().HasEntry("name"))
    {
        auto name = node.GetMetadata().GetEntry<std::string>("name");
        if (name.size() > 0)
        {
            typeName += "&#xa;" + name;
        }
    }
    properties.push_back(NameValue{ "Label", typeName });
    return typeName;
}

std::vector<NameValue> InspectNodeProperties(const Node& node)
{
    std::vector<NameValue> result;
    utilities::PropertyBag properties = node.GetMetadata();

    for (auto key : properties.Keys())
    {
        std::string value = properties[key].ToString();
        result.push_back(NameValue{ key, value });
    }

    GetNodeLabel(node, result);

    std::string inputShapes;
    for (int i = 0; i < node.NumInputPorts(); i++)
    {
        std::string inputShape = PrintActiveSize(node.GetInputPort(i));
        if (!inputShapes.empty())
        {
            inputShapes += ", ";
        }
        inputShapes += inputShape;
    }
    result.push_back(NameValue{ "inputs", inputShapes });

    std::string outputShapes;
    for (int i = 0; i < node.NumOutputPorts(); i++)
    {
        std::string outputShape = PrintActiveSize(node.GetOutputPort(i));
        if (!outputShapes.empty())
        {
            outputShapes += ", ";
        }
        outputShapes += outputShape;
    }
    result.push_back(NameValue{ "outputs", outputShapes });

    std::string typeName = node.GetRuntimeTypeName();
    std::vector<std::string> parts = Split(typeName, '<');
    if (parts.size() == 2)
    {
        std::string typeName = parts[1];
        ReplaceAll(typeName, "<", "");
        ReplaceAll(typeName, ">", "");
        result.push_back(NameValue{ "type", typeName }); // float or double
    }

    return result;
}

template <typename ValueType>
void PrintPredictorLayers(Graph& graph, GraphNode& childNode, const Node& node)
{
    const NeuralNetworkPredictorNode<ValueType>& predictorNode = dynamic_cast<const NeuralNetworkPredictorNode<ValueType>&>(node);
    auto predictor = predictorNode.GetPredictor();
    auto layers = predictor.GetLayers();
    GraphNode& previousLayer = childNode;
    int layerId = 0;
    for (auto ptr = layers.begin(), end = layers.end(); ptr != end; ptr++, layerId++)
    {
        std::shared_ptr<ell::predictors::neural::Layer<ValueType>> layer = *ptr;
        std::string layerName = layer->GetRuntimeTypeName();
        GraphNode& layerNode = graph.GetOrCreateNode(layerName + "(" + std::to_string(layerId) + ")", layerName);
        std::vector<NameValue> result = InspectLayerParameters<ValueType>(*layer);
        for (auto ptr = result.begin(), end = result.end(); ptr != end; ptr++)
        {
            NameValue nv = *ptr;
            layerNode.SetProperty(nv.name, nv.value);
        }

        graph.GetOrCreateLink(previousLayer, layerNode, "", "dependson");
        previousLayer = layerNode; // chain them together.
    }
}

void PrintGraph(const Model& model, const std::string& outputFormat, std::ostream& out, bool includeNodeId)
{
    // dump a graph of model in either DGML or DOT format.
    Graph graph;
    model.Visit([&](const Node& node) {
        std::string typeName = node.GetRuntimeTypeName();
        GraphNode& childNode = graph.GetOrCreateNode(to_string(node.GetId()), "");

        if (typeName == "NeuralNetworkPredictorNode<float>")
        {
            PrintPredictorLayers<float>(graph, childNode, node);
        }
        else if (typeName == "NeuralNetworkPredictorNode<double>")
        {
            PrintPredictorLayers<double>(graph, childNode, node);
        }
        else
        {
            std::string linkLabel;
            std::vector<NameValue> result = InspectNodeProperties(node);
            for (auto ptr = result.begin(), end = result.end(); ptr != end; ptr++)
            {
                NameValue nv = *ptr;
                if (nv.name == "Label")
                {
                    childNode.SetLabel(nv.value);
                }
                else
                {
                    childNode.SetProperty(nv.name, nv.value);
                }
                if (nv.name == "outputs")
                {
                    linkLabel = nv.value;
                }
            }
            auto dependencies = node.GetDependentNodes();
            for (auto ptr = dependencies.begin(), end = dependencies.end(); ptr != end; ptr++)
            {
                const Node* upstream = *ptr;
                if (upstream != nullptr)
                {
                    GraphNode& nextNode = graph.GetOrCreateNode(to_string(upstream->GetId()), "");
                    graph.GetOrCreateLink(childNode, nextNode, linkLabel, "");
                }
            }
        }
    });

    if (outputFormat == "dgml")
    {
        graph.SaveDgml(out);
    }
    else
    {
        graph.SaveDot(out);
    }
}
} // namespace ell
