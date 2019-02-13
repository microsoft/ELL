////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetConvolutionMethodTransformation.cpp (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SetConvolutionMethodTransformation.h"

#include <model/include/ModelTransformer.h>

#include <nodes/include/ConvolutionalLayerNode.h>

#include <predictors/neural/include/ConvolutionalLayer.h>

#include <utilities/include/Exception.h>

using namespace ell::model;

namespace ell
{
namespace passes
{
    namespace
    {
        template <typename Container, typename Function>
        auto Transform(const Container& container, Function fn)
        {
            std::vector<decltype(fn(container[0]))> result;
            result.reserve(container.size());
            std::transform(container.begin(), container.end(), std::back_inserter(result), fn);
            return result;
        }

        std::vector<const OutputPortBase*> GetReferencedPorts(const std::vector<const InputPortBase*>& inputs)
        {
            return Transform(inputs, [](auto input) { return &input->GetReferencedPort(); });
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

            transformer.CopyNode(node);
        }
    } // namespace

    // UseSimpleConvolutionTransformation
    Submodel UseSimpleConvolutionTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        auto onto = GetReferencedPorts(submodel.GetInputPorts());
        auto result = transformer.TransformSubmodelOnto(submodel, onto, context, [](const Node& node, ModelTransformer& transformer) {
            SetConvolutionMethod(node, transformer, PreferredConvolutionMethod::simple);
        });

        return result;
    }

    // UseUnrolledConvolutionTransformation
    Submodel UseUnrolledConvolutionTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        auto onto = GetReferencedPorts(submodel.GetInputPorts());
        auto result = transformer.TransformSubmodelOnto(submodel, onto, context, [](const Node& node, ModelTransformer& transformer) {
            SetConvolutionMethod(node, transformer, PreferredConvolutionMethod::unrolled);
        });

        return result;
    }

    // UseWinogradConvolutionTransformation
    Submodel UseWinogradConvolutionTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        auto onto = GetReferencedPorts(submodel.GetInputPorts());
        auto result = transformer.TransformSubmodelOnto(submodel, onto, context, [](const Node& node, ModelTransformer& transformer) {
            SetConvolutionMethod(node, transformer, PreferredConvolutionMethod::winograd);
        });

        return result;
    }

    // UseDiagonalConvolutionTransformation
    Submodel UseDiagonalConvolutionTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        auto onto = GetReferencedPorts(submodel.GetInputPorts());
        auto result = transformer.TransformSubmodelOnto(submodel, onto, context, [](const Node& node, ModelTransformer& transformer) {
            SetConvolutionMethod(node, transformer, PreferredConvolutionMethod::diagonal);
        });

        return result;
    }

    //
    // SetConvolutionMethodTransformation methods
    //
    Submodel SetConvolutionMethodTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        // TODO: traverse submodel
        // for each ConvolutionalLayerNode:
        //   get preferred algorithm from settings / metadata
        //   copy node, setting preferred algorithm in node properties

        // NOTE: this transformation could just be `Refine`, if the preferred method is already encoded
        // in the info that is seen by Refine anyway. Move it there, and just call Refine here.

        // auto preferredMethod = settings.optimizerSettings.preferredConvolutionMethod;
        // SetConvolutionMethod(node, context.GetTransformer(), preferredMethod);
        return submodel;
    }
} // namespace passes
} // namespace ell
