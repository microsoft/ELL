////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Transformation.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelTransformer.h"
#include "Submodel.h"
#include "TransformContext.h"

#include <string>

namespace ell
{
namespace model
{
    /// <summary> Base class for a global model optimizer transformation </summary>
    class Transformation
    {
    public:
        virtual ~Transformation() = default;

        /// <summary> Transforms a submodel using the given transformer </summary>
        virtual Submodel Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const = 0;

        /// <summary> Transforms a model using the given transformer. </summary>
        Model TransformModel(const Model& model, ModelTransformer& transformer, const TransformContext& context) const;

        /// <summary> Gets the name of this type. </summary>
        virtual std::string GetRuntimeTypeName() const = 0;
    };
} // namespace model
} // namespace ell
