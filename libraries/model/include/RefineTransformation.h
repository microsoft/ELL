////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RefineTransformation.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Model.h"
#include "Submodel.h"
#include "Transformation.h"

#include <vector>

namespace ell
{
namespace model
{
    /// <summary> A transformation that refines the nodes in the given submodel. </summary>
    class RefineTransformation : public model::Transformation
    {
    public:
        /// <summary> Constructor. </summary>
        ///
        /// <param name="maxIterations"> The maximum number of refinement iterations to perform. </param>
        RefineTransformation(int maxIterations = 10);

        /// <summary> Refine the nodes in the submodel if possible. </summary>
        model::Submodel Transform(const model::Submodel& submodel, model::ModelTransformer& transformer, const TransformContext& context) const override;

        /// <summary> Gets the name of this type. </summary>
        static std::string GetTypeName() { return "RefineTransformation"; }

        /// <summary> Gets the name of this type. </summary>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        int _maxIterations;
    };
} // namespace model
} // namespace ell
