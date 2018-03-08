////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FuseLinearOperationsPass.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"

// model/optimizer
#include "ModelOptimizer.h"
#include "OptimizationPass.h"

// stl
#include <vector>

namespace ell
{
namespace passes
{
    /// <summary> An optimization pass that fuses sequential `BroadcastLinearFunctionNode`s into one. </summary>
    class FuseLinearOperationsPass : public model::NodeLocalOptimizationPass
    {
    public:
        /// <summary> Combine a linear function node with its predecessor if possible. </summary>
        ///
        /// <param name="node"> The current node being visited. </param>
        /// <param name="transformer"> The transformer object operating on the model. </param>
        void OptimizeNode(const model::Node& node, model::ModelTransformer& transformer) override;

        /// <summary> Add this pass type to the global pass registry. </summary>
        static void AddToRegistry();
    };
}
}

