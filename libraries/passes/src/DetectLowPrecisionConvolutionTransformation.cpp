////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DetectLowPrecisionConvolutionTransformation.cpp (passes)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DetectLowPrecisionConvolutionTransformation.h"

#include <model/include/ModelTransformer.h>
#include <model/include/RefineTransformation.h>

#include <nodes/include/BinaryConvolutionalLayerNode.h>
#include <nodes/include/ConvolutionalLayerNode.h>

#include <predictors/neural/include/BinaryConvolutionalLayer.h>
#include <predictors/neural/include/ConvolutionalLayer.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Logger.h>
#include <utilities/include/StlVectorUtil.h>

#include <cmath>
#include <set>
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
        enum class DetectedWeightType
        {
            Unknown,
            FullPrecision,
            Binary,
            SignedMean,
            Ternary
        };

        template <typename Container, typename Function>
        auto Transform(const Container& container, Function fn)
        {
            return utilities::TransformVector(container.begin(), container.end(), fn);
        }

        std::vector<const OutputPortBase*> GetReferencedPorts(const std::vector<const InputPortBase*>& inputs)
        {
            return Transform(inputs, [](auto input) { return &input->GetReferencedPort(); });
        }

        bool IsConvolutionalLayerNode(const Node& node)
        {
            return (node.GetRuntimeTypeName().find("ConvolutionalLayerNode") == 0);
        }

        template <typename ValueType>
        DetectedWeightType DetectWeightType(const nodes::ConvolutionalLayerNode<ValueType>* node, model::ModelTransformer& transformer)
        {
            DetectedWeightType detectedWeightType{ DetectedWeightType::Unknown };
            auto layer = node->GetLayer();
            auto weights = node->GetLayer().GetWeights();

            const int numFilters = layer.GetOutputShape().NumChannels();

            // Skip convolutions that have only a single input channel e.g. in the spatial portion
            // of depthwise separable convolutions
            if (weights.NumChannels() == 1)
            {
                return DetectedWeightType::FullPrecision;
            }

            // Perform detection based on each filter, since for xnor, each filter is mean or -mean.
            // Later, we confirm that each filter detects the same type.
            for (int filter = 0; filter < numFilters; ++filter)
            {
                // Keep track of unique values for this filter
                std::set<ValueType> uniqueValues;
                const size_t numChannels = weights.NumChannels();
                const size_t numRows = layer.GetConvolutionalParameters().receptiveField;

                for (size_t channel = 0; channel < numChannels; ++channel)
                {
                    for (size_t row = 0; row < numRows; ++row)
                    {
                        // Filters are stacked in the row dimension
                        const int weightRow = row + (filter * numRows);

                        for (size_t column = 0; column < layer.GetConvolutionalParameters().receptiveField; ++column)
                        {
                            uniqueValues.insert(weights(weightRow, column, channel));
                        }
                        if (uniqueValues.size() > 3)
                        {
                            break;
                        }
                    }
                }

                // Binary or ternary weights should only have 2 or 3 unique values respectively
                if (uniqueValues.size() < 2 || uniqueValues.size() > 3)
                {
                    return DetectedWeightType::FullPrecision;
                }

                DetectedWeightType proposedWeightType{ DetectedWeightType::FullPrecision };

                // Retrieve unique values as a vector
                std::vector<ValueType> values(uniqueValues.begin(), uniqueValues.end());
                if (values.size() == 2) // If the number of unique values is 2, check for binary or XNOR
                {
                    const ValueType x = values[0];
                    const ValueType y = values[1];

                    // If the number of uniques values is 2, check for binary or XNor.
                    // Binary is -1 or 1, XNor is -mean, mean
                    if (std::abs(x) == 1 && (x == -y))
                    {
                        // Values are 1 and -1
                        proposedWeightType = DetectedWeightType::Binary;
                    }
                    else if (x == -y)
                    {
                        // Values are mean and -mean
                        proposedWeightType = DetectedWeightType::SignedMean;
                    }
                }
                else if (values.size() == 3) // If the number of uniques values is 3, check for ternary. Ternary values are -1, 0, 1.
                {
                    bool ternary = true;
                    for (const auto value : values)
                    {
                        if (value != 1 && value != -1 && value != 0) ternary = false;
                    }
                    if (ternary)
                    {
                        // Values are -1, 0, or 1
                        proposedWeightType = DetectedWeightType::Ternary;
                    }
                }

                // Verify that the poposed weight type this is the same as the previously detected weight type.
                if (detectedWeightType == DetectedWeightType::Unknown)
                {
                    detectedWeightType = proposedWeightType;
                }
                else if (proposedWeightType != detectedWeightType)
                {
                    return DetectedWeightType::FullPrecision;
                }
            }

            return detectedWeightType;
        }

        // returns 'true' if we handled the situation, else 'false'. If we return 'false', keep trying other ValueTypes.
        template <typename ValueType>
        bool TryReducedBitConvolution(const model::Node& node, model::ModelTransformer& transformer)
        {
            using ell::predictors::neural::BinaryConvolutionMethod;
            using ell::predictors::neural::BinaryWeightsScale;

            auto thisNode = dynamic_cast<const nodes::ConvolutionalLayerNode<ValueType>*>(&node);
            if (thisNode == nullptr)
            {
                return false;
            }

            auto detectedWeightType = DetectWeightType<ValueType>(thisNode, transformer);
            if (detectedWeightType == DetectedWeightType::FullPrecision)
            {
                return false;
            }

            const auto& newInput = transformer.GetCorrespondingInputs(thisNode->input);
            const auto& layer = thisNode->GetLayer();

            auto layerParameters = layer.GetLayerParameters();
            auto convolutionalParameters = layer.GetConvolutionalParameters();

            if (detectedWeightType == DetectedWeightType::Binary || detectedWeightType == DetectedWeightType::SignedMean)
            {
                predictors::neural::BinaryConvolutionalParameters binConvolutionalParameters{
                    convolutionalParameters.receptiveField,
                    convolutionalParameters.stride,
                    BinaryConvolutionMethod::bitwise,
                    (detectedWeightType == DetectedWeightType::SignedMean) ? BinaryWeightsScale::mean : BinaryWeightsScale::none
                };

                predictors::neural::BinaryConvolutionalLayer<ValueType> newLayer = { layerParameters, binConvolutionalParameters, layer.GetWeights() };

                auto newNode = transformer.AddNode<nodes::BinaryConvolutionalLayerNode<ValueType>>(newInput, newLayer);
                newNode->GetMetadata() = node.GetMetadata();

                Log() << "Detected Binary Convolution for node " << thisNode->GetId() << std::endl;
                transformer.MapNodeOutput(thisNode->output, newNode->output);
            }
            else
            {
                Log() << "Detected Ternary Convolution for node " << thisNode->GetId() << std::endl;
                Log() << "Not implemented yet, so using normal Convolution instead" << std::endl;
                transformer.CopyNode(node);
            }

            return true;
        }

        void DetectReducedBitConvolution(const model::Node& node, model::ModelTransformer& transformer)
        {
            if (TryReducedBitConvolution<float>(node, transformer))
            {
                return;
            }
            if (TryReducedBitConvolution<double>(node, transformer))
            {
                return;
            }

            transformer.CopyNode(node);
        }
    } // namespace

    //
    // DetectReducedBitConvolutions methods
    //
    Submodel DetectLowPrecisionConvolutionTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        // Now set the method on any ConvolutionalLayerNodes, using an in-place transformation
        auto onto = transformer.GetCorrespondingOutputs(GetReferencedPorts(submodel.GetInputs()));
        model::Model destModel = submodel.GetModel().ShallowCopy();
        auto result = transformer.TransformSubmodelOnto(submodel, destModel, onto, context, [context](const Node& node, ModelTransformer& transformer) {
            if (IsConvolutionalLayerNode(node))
            {
                DetectReducedBitConvolution(node, transformer);
            }
            else
            {
                transformer.CopyNode(node);
            }
        });
        return result;
    }
} // namespace passes
} // namespace ell
