////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintModel.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintModel.h"
#include "LayerInspector.h"

#include <utilities/include/Exception.h>
#include <utilities/include/OutputStreamImpostor.h>

#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/Node.h>
#include <model/include/PortElements.h>

#include <nodes/include/NeuralNetworkPredictorNode.h>

#include <iostream>

namespace ell
{
template <typename ValueType>
void PrintLayerParameters(std::ostream& out, const ell::predictors::neural::Layer<ValueType>& layer)
{
    std::vector<NameValue> result = InspectLayerParameters<ValueType>(layer);
    bool first = true;
    for (auto ptr = result.begin(), end = result.end(); ptr != end; ++ptr)
    {
        NameValue nv = *ptr;
        if (!first)
        {
            out << ", ";
        }
        out << nv.name << "=" << nv.value;
        first = false;
    }
}

template <typename ValueType>
void PrintNeuralNetworkPredictorNode(std::ostream& out, const model::Node& node)
{
    const ell::nodes::NeuralNetworkPredictorNode<ValueType>& predictorNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<ValueType>&>(node);
    auto predictor = predictorNode.GetPredictor();
    auto layers = predictor.GetLayers();
    for (auto ptr = layers.begin(), end = layers.end(); ptr != end; ++ptr)
    {
        std::shared_ptr<ell::predictors::neural::Layer<ValueType>> layer = *ptr;
        std::string layerName = layer->GetRuntimeTypeName();
        out << "        ";
        PrintLayerParameters(out, *layer);
    }
    out << "\n";
}

void PrintNodeDetails(std::ostream& out, const ell::model::Node& node)
{
    auto nodeType = node.GetRuntimeTypeName();
    if (nodeType == "NeuralNetworkPredictorNode<float>")
    {
        PrintNeuralNetworkPredictorNode<float>(out, node);
    }
    else if (nodeType == "NeuralNetworkPredictorNode<double>")
    {
        PrintNeuralNetworkPredictorNode<double>(out, node);
    }
    else if (const ell::nodes::NeuralNetworkLayerNodeBase<float>* layerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<float>*>(&node))
    {
        // model Visit doesn't look inside this node...
        auto& layer = layerNode->GetBaseLayer();
        out << "    ";
        PrintLayerParameters(out, layer);
    }
    else if (const ell::nodes::NeuralNetworkLayerNodeBase<double>* layerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<double>*>(&node))
    {
        // model Visit doesn't look inside this node...
        auto& layer = layerNode->GetBaseLayer();
        out << "    ";
        PrintLayerParameters(out, layer);
    }
}

void PrintNode(const model::Node& node, std::ostream& out, const PrintModelOptions& options)
{
    if (options.includeNodeId)
    {
        out << "<id:" << node.GetId() << "> ";
    }

    auto nodeType = node.GetRuntimeTypeName();
    out << nodeType;

    bool isFirstPort = true;
    out << "(";
    for (const auto& inputPort : node.GetInputPorts())
    {
        out << (isFirstPort ? "" : ", ");
        isFirstPort = false;

        out << inputPort->GetReferencedPort().GetNode()->GetId() << ".";
        out << inputPort->GetReferencedPort().GetName();
        auto size = inputPort->GetMemoryLayout().GetActiveSize();
        out << "[" << size << "]";
    }
    out << ")";

    if (node.NumOutputPorts() > 0)
    {
        out << " -> ";
        isFirstPort = true;
        for (const auto& outputPort : node.GetOutputPorts())
        {
            out << (isFirstPort ? "" : ", ");
            isFirstPort = false;
            out << outputPort->GetName();
            auto size = outputPort->GetMemoryLayout().GetActiveSize();
            out << "[" << size << "]";
        }
    }

    if (options.nodeDetails)
    {
        PrintNodeDetails(out, node);
    }
    out << std::endl;
}

void PrintModel(const model::Model& model, std::ostream& out, const PrintModelOptions& options)
{
    model.Visit([&out, options](const model::Node& node) { PrintNode(node, out, options); });
}
} // namespace ell
