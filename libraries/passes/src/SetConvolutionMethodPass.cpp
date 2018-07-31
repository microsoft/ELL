////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetConvolutionMethodPass.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SetConvolutionMethodPass.h"

// model
#include "ModelTransformer.h"
#include "OptimizationPassRegistry.h"

// nodes
#include "ConvolutionalLayerNode.h"

// predictors/neural
#include "ConvolutionalLayer.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace passes
{
    namespace
    {
        predictors::neural::ConvolutionMethod GetConvolutionMethod(model::PreferredConvolutionMethod preferredMethod)
        {
            switch(preferredMethod)
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
            if(method == predictors::neural::ConvolutionMethod::winograd)
            {
                if(convolutionalParameters.stride != 1)
                {
                    return false;
                }
                if(convolutionalParameters.receptiveField != 3)
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

            auto newInput = transformer.TransformPortElements(thisNode->input.GetPortElements());
            const auto& layer = thisNode->GetLayer();

            auto layerParameters = layer.GetLayerParameters();
            auto convolutionalParameters = layer.GetConvolutionalParameters();

            auto method = GetConvolutionMethod(preferredMethod);
            convolutionalParameters.method = method;
            if(!IsMethodCompatible(method, convolutionalParameters))
            {
                return false;
            }
            predictors::neural::ConvolutionalLayer<ValueType> newLayer = { layerParameters, convolutionalParameters, layer.GetWeights() };

            auto newNode = transformer.AddNode<nodes::ConvolutionalLayerNode<ValueType>>(newInput, newLayer);

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

            node.Copy(transformer);
        }
    }

    //
    // SetConvolutionMethodPass methods
    //
    void SetConvolutionMethodPass::OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const
    {
        auto preferredMethod = settings.optimizerSettings.preferredConvolutionMethod;
        SetConvolutionMethod(node, context.GetTransformer(), preferredMethod);
    }

    void SetConvolutionMethodPass::AddToRegistry()
    {
        model::OptimizationPassInfo info = {
            "SetConvolutionMethodPass",
            [](const model::ModelOptimizerOptions& settings) { return settings.preferredConvolutionMethod != model::PreferredConvolutionMethod::automatic; },
            [] { return std::make_unique<SetConvolutionMethodPass>(); }
        };
        model::OptimizationPassRegistry::AddPass(info);
    }
}
}
