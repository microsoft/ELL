////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetConvolutionMethodTransformation.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SetConvolutionMethodTransformation.h"

#include <model/include/ModelTransformer.h>
#include <model/include/RefineTransformation.h>

#include <nodes/include/ConvolutionalLayerNode.h>

#include <predictors/neural/include/ConvolutionalLayer.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Logger.h>
#include <utilities/include/StlVectorUtil.h>

#include <vector>

namespace ell
{
namespace passes
{
    using namespace model;
    using namespace utilities::logging;
    using utilities::logging::Log;

    namespace
    {
        template <typename Container, typename Function>
        auto Transform(const Container& container, Function fn)
        {
            return utilities::TransformVector(container.begin(), container.end(), fn);
        }

        std::vector<const OutputPortBase*> GetReferencedPorts(const std::vector<const InputPortBase*>& inputs)
        {
            return Transform(inputs, [](auto input) { return &input->GetReferencedPort(); });
        }

        bool IsNeuralNetworkPredictorNode(const Node& node)
        {
            return (node.GetRuntimeTypeName().find("NeuralNetworkPredictorNode") == 0);
        }

        bool IsConvolutionalLayerNode(const Node& node)
        {
            return (node.GetRuntimeTypeName().find("ConvolutionalLayerNode") == 0);
        }

        predictors::neural::ConvolutionMethod GetConvolutionMethod(model::PreferredConvolutionMethod preferredMethod)
        {
            switch (preferredMethod)
            {
            case model::PreferredConvolutionMethod::unrolled:
                return predictors::neural::ConvolutionMethod::unrolled;
            case model::PreferredConvolutionMethod::simple:
                return predictors::neural::ConvolutionMethod::simple;
            case model::PreferredConvolutionMethod::diagonal:
                return predictors::neural::ConvolutionMethod::diagonal;
            case model::PreferredConvolutionMethod::winograd:
                return predictors::neural::ConvolutionMethod::winograd;
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
            }
        }

        bool IsMethodCompatible(predictors::neural::ConvolutionMethod method, const predictors::neural::ConvolutionalParameters& convolutionalParameters)
        {
            if (method == predictors::neural::ConvolutionMethod::winograd)
            {
                if (convolutionalParameters.stride != 1)
                {
                    return false;
                }
                if (convolutionalParameters.receptiveField != 3)
                {
                    return false;
                }
            }
            return true;
        }

        // returns 'true' if we handled the situation, else 'false'. If we return 'false', keep trying other ValueTypes.
        template <typename ValueType>
        bool TrySetConvolutionMethod(const model::Node& node, model::ModelTransformer& transformer, model::PreferredConvolutionMethod preferredMethod)
        {
            auto thisNode = dynamic_cast<const nodes::ConvolutionalLayerNode<ValueType>*>(&node);
            if (thisNode == nullptr)
            {
                return false;
            }

            const auto& newInput = transformer.GetCorrespondingInputs(thisNode->input);
            const auto& layer = thisNode->GetLayer();

            auto layerParameters = layer.GetLayerParameters();
            auto convolutionalParameters = layer.GetConvolutionalParameters();

            auto method = GetConvolutionMethod(preferredMethod);
            convolutionalParameters.method = method;
            if (!IsMethodCompatible(method, convolutionalParameters))
            {
                Log() << "Invalid convolution method: " << static_cast<int>(method) << " for node " << thisNode->GetId() << std::endl;
                return false;
            }
            predictors::neural::ConvolutionalLayer<ValueType> newLayer = { layerParameters, convolutionalParameters, layer.GetWeights() };

            // TODO: just copy the node and modify its layer
            auto newNode = transformer.AddNode<nodes::ConvolutionalLayerNode<ValueType>>(newInput, newLayer);
            newNode->GetMetadata() = node.GetMetadata();

            Log() << "Setting convolution method to " << static_cast<int>(method) << " for node " << thisNode->GetId() << std::endl;
            transformer.MapNodeOutput(thisNode->output, newNode->output);
            return true;
        }

        void SetConvolutionMethod(const model::Node& node, model::ModelTransformer& transformer, model::PreferredConvolutionMethod preferredMethod)
        {
            if (preferredMethod != model::PreferredConvolutionMethod::automatic)
            {
                if (TrySetConvolutionMethod<float>(node, transformer, preferredMethod))
                {
                    return;
                }
                if (TrySetConvolutionMethod<double>(node, transformer, preferredMethod))
                {
                    return;
                }
            }

            transformer.CopyNode(node);
        }
    } // namespace

    //
    // SetConvolutionMethodTransformation methods
    //
    Submodel SetConvolutionMethodTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        // NOTE: this transformation could be taken care of in `ConvolutionalLayerNode::Refine`, if the preferred method is already encoded
        // in the info that is seen by Refine anyway. Maybe just move it there, and just call Refine here. Then have a different transformation
        // that sets the metadata.

        // First refine any NeuralNetworkPredictorNodes
        auto refineNNPredictorFn = [](const model::Node& node) {
            return IsNeuralNetworkPredictorNode(node) ? model::NodeAction::refine : model::NodeAction::compile;
        };
        model::TransformContext refineNNPredictorContext{ refineNNPredictorFn };
        RefineTransformation refineTransformation;
        auto result1 = refineTransformation.Transform(submodel, transformer, refineNNPredictorContext);

        // Now set the method on any ConvolutionalLayerNodes, using an in-place transformation
        auto onto = transformer.GetCorrespondingOutputs(GetReferencedPorts(result1.GetInputs()));
        model::Model destModel = result1.GetModel().ShallowCopy();
        auto result2 = transformer.TransformSubmodelOnto(result1, destModel, onto, context, [context](const Node& node, ModelTransformer& transformer) {
            model::PreferredConvolutionMethod preferredMethod = model::PreferredConvolutionMethod::automatic;
            auto compiler = context.GetCompiler();
            if (compiler)
            {
                preferredMethod = compiler->GetModelOptimizerOptions(node).GetEntry<PreferredConvolutionMethod>("preferredConvolutionMethod", PreferredConvolutionMethod::automatic);
            }

            SetConvolutionMethod(node, transformer, preferredMethod);
        });

        // Finally, refine any ConvolutionalLayerNodes
        auto refineConvLayerFn = [](const model::Node& node) {
            return IsConvolutionalLayerNode(node) ? model::NodeAction::refine : model::NodeAction::compile;
        };
        model::TransformContext refineConvLayerContext{ refineConvLayerFn };
        auto result3 = refineTransformation.Transform(result2, transformer, refineConvLayerContext);
        return result3;
    }
} // namespace passes
} // namespace ell
