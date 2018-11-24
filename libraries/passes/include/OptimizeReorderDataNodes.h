////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeReorderDataNodes.h (passes)
//  Authors:  Kern Handa
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
    /// <summary> An optimization pass that removes unnecessary `ReorderDataNode`s. </summary>
    class OptimizeReorderDataNodes : public model::NodeLocalOptimizationPass
    {
    public:
        OptimizeReorderDataNodes();

        ~OptimizeReorderDataNodes();

        /// <summary> Remove or replace with one a sequence of `ReorderDataNode`s. </summary>
        ///
        /// <param name="node"> The current node being visited. </param>
        /// <param name="transformer"> The transformer object operating on the model. </param>
        void OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const override;

        /// <summary> Add this pass type to the global pass registry. </summary>
        static void AddToRegistry();

    private:
        struct State;
        std::unique_ptr<State> _state;
    };
} // namespace passes
} // namespace ell
