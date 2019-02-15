////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Transformation.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Transformation.h"

namespace ell
{
namespace model
{
    Model Transformation::TransformModel(const Model& model, ModelTransformer& transformer, const TransformContext& context) const
    {
        model::Submodel oldSubmodel{ model };
        auto newSubmodel = Transform(oldSubmodel, transformer, context);
        return newSubmodel.GetModel().ShallowCopy();
    }
} // namespace model
} // namespace ell
