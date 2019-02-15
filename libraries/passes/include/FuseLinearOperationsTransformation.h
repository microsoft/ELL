////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FuseLinearOperationsTransformation.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/ModelTransformer.h>
#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

namespace ell
{
namespace passes
{
    class FuseLinearOperationsTransformation : public ell::model::Transformation
    {
    public:
        ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

        std::string GetRuntimeTypeName() const override
        {
            return "FuseLinearOperationsTransformation";
        }

    private:
    };
} // namespace passes
} // namespace ell
