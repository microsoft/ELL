////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeModelTransformation.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Model.h"
#include "Submodel.h"
#include "Transformation.h"

#include <model/optimizer/include/ModelOptimizer.h>

#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A transformation that invokes the ModelOptimizer on the given submodel. </summary>
    class OptimizeModelTransformation : public model::optimizer::Transformation
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="optimizer"> The ModelOptimizer to run on the model. </param>
        OptimizeModelTransformation(const ModelOptimizer& optimizer);

        /// <summary> Optimize the model. </summary>
        model::Submodel Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "OptimizeModelTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        const ModelOptimizer& _optimizer;
    };
} // namespace model
} // namespace ell
