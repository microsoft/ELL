////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintGraph.cpp (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LayerInspector.h"
#include "PrintModel.h"

// utilities
#include "Exception.h"
#include "OutputStreamImpostor.h"

// model
#include "Graph.h"
#include "InputPort.h"
#include "Model.h"
#include "NeuralNetworkPredictorNode.h"
#include "Node.h"
#include <iostream>
#include <string>

using namespace ell::utilities;

namespace ell
{
extern std::string PaddingSchemeToString(ell::predictors::neural::PaddingScheme scheme);

void PrintGraph(const model::Model& model, const std::string& outputFormat, std::ostream& out, bool includeNodeId)
{
    // dump DGML graph of model
    Graph graph;
    model.Visit([&](const model::Node& node) {
        std::string typeName = node.GetRuntimeTypeName();
        std::string label = typeName;
        if (includeNodeId)
        {
            label.insert(0, "<id:" + to_string(node.GetId()) + ">");
        }
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
                std::vector<NameValue> result = InspectLayerParameters<float>(layer);
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
            auto dependencies = node.GetDependentNodes();
            for (auto ptr = dependencies.begin(), end = dependencies.end(); ptr != end; ptr++)
            {
                const model::Node* upstream = *ptr;
                if (upstream != nullptr)
                {
                    label = upstream->GetRuntimeTypeName();
                    if (includeNodeId)
                    {
                        label.insert(0, "<id:" + to_string(upstream->GetId()) + ">");
                    }
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
}
