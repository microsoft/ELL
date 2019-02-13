////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RefineAndOptimizeTransformation.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRMapCompiler.h"
#include "Model.h"
#include "Submodel.h"
#include "Transformation.h"

#include <model/optimizer/include/ModelOptimizer.h>

#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A transformation that refines and optimizes the given submodel. </summary>
    class RefineAndOptimizeTransformation : public model::optimizer::Transformation
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="optimizer"> The ModelOptimizer to run on the model. </param>
        /// <param name="maxIterations"> The maximum number of refinement iterations to perform. </param>
        RefineAndOptimizeTransformation(const ModelOptimizer& optimizer, int maxIterations = 10);

        /// <summary> Refine the nodes in the submodel if possible. </summary>
        model::Submodel Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "RefineAndOptimizeTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        const ModelOptimizer& _optimizer;
        int _maxIterations;
    };
} // namespace model
} // namespace ell
