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

class FuseLinearOperationsTransformation : public ell::model::optimizer::Transformation
{
public:
    ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

    /// <summary> Gets the name of this type. </summary>
    static std::string GetTypeName() { return "FuseLinearOperationsTransformation"; }

    /// <summary> Gets the name of this type. </summary>
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
};
