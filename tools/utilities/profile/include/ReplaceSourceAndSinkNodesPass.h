////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReplaceSourceAndSinkNodesPass.h (profile)
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
/// <summary> An optimization pass that replaces `SourceNode`s with `InputNode`s and `SinkNode`s with `OutputNode`s </summary>
class ReplaceSourceAndSinkNodesPass : public model::NodeLocalOptimizationPass
{
public:
    /// <summary> Replace a source or sink node with an input or output node. </summary>
    ///
    /// <param name="node"> The current node being visited. </param>
    /// <param name="transformer"> The transformer object operating on the model. </param>
    void OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const override;

    /// <summary> Add this pass type to the global pass registry. </summary>
    static void AddToRegistry();
};
}
