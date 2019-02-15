////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReplaceSourceAndSinkNodesTransformation.h (debugCompiler)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Transformation.h>

namespace ell
{
/// <summary> A transformation that replaces `SourceNode`s with `InputNode`s and `SinkNode`s with `OutputNode`s </summary>
class ReplaceSourceAndSinkNodesTransformation : public model::Transformation
{
public:
    /// <summary> Set the convolution algorithm for the `ConvolutionalLayerNode`s if possible. </summary>
    model::Submodel Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

    /// <summary> Returns the ID for this transformation </summary>
    std::string GetRuntimeTypeName() const override { return { "ReplaceSourceAndSinkNodesTransformationb" }; };
};
} // namespace ell
