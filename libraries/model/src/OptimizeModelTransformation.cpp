////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeModelTransformation.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizeModelTransformation.h"
#include "TransformationRegistry.h"

namespace ell
{
namespace model
{
    Submodel OptimizeModelTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        Submodel result = submodel;
        const auto& registry = TransformationRegistry::GetGlobalRegistry();

        for (const auto& transformation : registry)
        {
            result = transformation->Transform(result, transformer, context);
        }
        return result;
    }
} // namespace model
} // namespace ell
