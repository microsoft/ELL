////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintModel.cpp (print)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintModel.h"

// utilities
#include "Exception.h"
#include "OutputStreamImpostor.h"

// model
#include "DgmlGraph.h"
#include "InputPort.h"
#include "Model.h"
#include "NeuralNetworkPredictorNode.h"
#include "Node.h"
#include <iostream>
#include <string>

namespace ell
{
void PrintGraph(const model::Model& model, std::ostream& out)
{
    // dump DGML graph of model
    DgmlGraph graph;
    model.Visit([&](const model::Node& node) {
        std::string typeName = node.GetRuntimeTypeName();
        DgmlNode childNode = graph.GetOrCreateNode(to_string(node.GetId()), typeName);

        if (typeName == "NeuralNetworkPredictorNode<float>")
        {
            const ell::nodes::NeuralNetworkPredictorNode<float>& predictorNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>&>(node);
            auto predictor = predictorNode.GetPredictor();
            auto layers = predictor.GetLayers();
            DgmlNode& previousLayer = childNode;
            int layerId = 0;
            for (auto ptr = layers.begin(), end = layers.end(); ptr != end; ptr++, layerId++)
            {
                std::shared_ptr<ell::predictors::neural::Layer<float>> layer = *ptr;
                auto shape = layer->GetLayerParameters().outputShape;
                std::string layerName = layer->GetRuntimeTypeName();
                DgmlNode& layerNode = graph.GetOrCreateNode(layerName + "(" + std::to_string(layerId) + ")", layerName);
                layerNode.SetProperty("shape", "[" + std::to_string(shape[0]) + "," + std::to_string(shape[1]) + "," + std::to_string(shape[2]) + "]");
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
                    DgmlNode& upstreamNode = graph.GetOrCreateNode(to_string(upstream->GetId()), upstream->GetRuntimeTypeName());
                    graph.GetOrCreateLink(upstreamNode, childNode, "dependson");
                }
            }
        }
    });

    graph.Save(out);
}
}