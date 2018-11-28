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

#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/Node.h>

#include <nodes/include/NeuralNetworkPredictorNode.h>

#include <iostream>
#include <string>

using namespace ell::utilities;
using namespace ell::model;

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
                shape += ",";
            }
            shape += std::to_string(i);
        }
    }
    return shape;
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
    if (node.NumInputPorts() > 0) 
    {
        std::string inputShape = PrintActiveSize(node.GetInputPort(0));
        result.push_back(NameValue{ "input", inputShape });
    }
    if (node.NumOutputPorts() > 0) 
    {
        std::string outputShape = PrintActiveSize(node.GetOutputPort(0));
        result.push_back(NameValue{ "output", outputShape });
    }
    return result;
}

void PrintGraph(const Model& model, const std::string& outputFormat, std::ostream& out, bool includeNodeId)
{
    // dump DGML graph of model
    Graph graph;
    model.Visit([&](const Node& node) {
        std::string typeName = node.GetRuntimeTypeName();
        std::string label = typeName;
        GraphNode& childNode = graph.GetOrCreateNode(to_string(node.GetId()), label);

        if (typeName == "NeuralNetworkPredictorNode<float>")
        {
            const ell::nodes::NeuralNetworkPredictorNode<float>& predictorNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>&>(node);
            auto predictor = predictorNode.GetPredictor();
            auto layers = predictor.GetLayers();
            GraphNode& previousLayer = childNode;
            int layerId = 0;
            for (auto ptr = layers.begin(), end = layers.end(); ptr != end; ptr++, layerId++)
            {
                std::shared_ptr<ell::predictors::neural::Layer<float>> layer = *ptr;
                std::string layerName = layer->GetRuntimeTypeName();
                GraphNode& layerNode = graph.GetOrCreateNode(layerName + "(" + std::to_string(layerId) + ")", layerName);
                std::vector<NameValue> result = InspectLayerParameters<float>(*layer);
                for (auto ptr = result.begin(), end = result.end(); ptr != end; ptr++)
                {
                    NameValue nv = *ptr;
                    layerNode.SetProperty(nv.name, nv.value);
                }

                graph.GetOrCreateLink(previousLayer, layerNode, "dependson");
                previousLayer = layerNode; // chain them together.
            }
        }
        else
        {
            std::vector<NameValue> result = InspectNodeProperties(node);
            for (auto ptr = result.begin(), end = result.end(); ptr != end; ptr++)
            {
                NameValue nv = *ptr;
                childNode.SetProperty(nv.name, nv.value);
            }

            auto dependencies = node.GetDependentNodes();
            for (auto ptr = dependencies.begin(), end = dependencies.end(); ptr != end; ptr++)
            {
                const Node* upstream = *ptr;
                if (upstream != nullptr)
                {
                    label = upstream->GetRuntimeTypeName();
                    GraphNode& nextNode = graph.GetOrCreateNode(to_string(upstream->GetId()), label);
                    graph.GetOrCreateLink(childNode, nextNode, "");
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
