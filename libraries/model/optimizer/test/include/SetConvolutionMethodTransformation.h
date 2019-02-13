////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetConvolutionMethodTransformation.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Model.h>
#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

#include <vector>

namespace ell
{
namespace passes
{
    /// <summary> A transformation that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class UseSimpleConvolutionTransformation : public ell::model::optimizer::Transformation
    {
    public:
        ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "UseSimpleConvolutionTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };

    /// <summary> A transformation that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class UseUnrolledConvolutionTransformation : public ell::model::optimizer::Transformation
    {
    public:
        ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "UseUnrolledConvolutionTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };

    /// <summary> A transformation that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class UseWinogradConvolutionTransformation : public ell::model::optimizer::Transformation
    {
    public:
        ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "UseWinogradConvolutionTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };

    /// <summary> A transformation that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class UseDiagonalConvolutionTransformation : public ell::model::optimizer::Transformation
    {
    public:
        ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "UseDiagonalConvolutionTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };

    /// <summary> A transformation that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class SetConvolutionMethodTransformation : public model::optimizer::Transformation
    {
    public:
        /// <summary> Set the convolution algorithm for the `ConvolutionalLayerNode`s if possible. </summary>
        model::Submodel Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "SetConvolutionMethodTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };
} // namespace passes
} // namespace ell
