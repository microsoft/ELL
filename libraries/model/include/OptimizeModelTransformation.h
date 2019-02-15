////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeModelTransformation.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Transformation.h"

namespace ell
{
namespace model
{
    /// <summary> A transformation that invokes the registered transformations on a submodel. </summary>
    class OptimizeModelTransformation : public Transformation
    {
    public:
        /// <summary> Optimize a submodel. </summary>
        ///
        /// <returns> Returns a new submodel that has had all the registered transformations applied to it. </returns>
        Submodel Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "OptimizeModelTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }
    };
} // namespace model
} // namespace ell
