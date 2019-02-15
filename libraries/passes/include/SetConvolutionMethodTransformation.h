////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetConvolutionMethodTransformation.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Transformation.h>

namespace ell
{
namespace passes
{
    /// <summary> A transformation that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class SetConvolutionMethodTransformation : public model::Transformation
    {
    public:
        /// <summary> Set the convolution algorithm for the `ConvolutionalLayerNode`s if possible. </summary>
        model::Submodel Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        /// <summary> Returns the ID for this transformation </summary>
        std::string GetRuntimeTypeName() const override { return { "SetConvolutionMethodTransformation" }; };
    };
} // namespace passes
} // namespace ell
