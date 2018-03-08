////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationPass.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace model
{
    class Model;
    class ModelOptimizer;
    class ModelTransformer;
    class Node;

    /// <summary> Base class for optimization passes </summary>
    class OptimizationPass
    {
    public:
        /// <summary> Perform any pre-optimization initialization required by the pass. </summary>
        /// This method is always called before the optimization pass is started.
        ///
        /// <param name="optimizer"> The optimizer this pass belongs to. </param>
        /// <param name="model"> The model about to be optimized. </param>
        virtual void Initialize(ModelOptimizer& optimizer, const Model& model) = 0;

        /// <summary> Run this pass. </summary>
        ///
        /// <param name="optimizer"> The optimizer this pass belongs to. </param>
        /// <param name="model"> The model being optimized. </param>
        virtual Model Run(ModelOptimizer& optimizer, const Model& model) = 0;

        /// <summary> Perform any post-optimization teardown required by the pass. </summary>
        /// This method is always called after the optimization pass is finished.
        ///
        /// <param name="optimizer"> The optimizer this pass belongs to. </param>
        /// <param name="model"> The (new) model that was the result of the optimization. </param>
        virtual void Finalize(ModelOptimizer& optimizer, const Model& model) = 0;
    };

    /// <summary> An optimization pass that operates on the local neighborhood of a node. </summary>
    ///
    /// This pass operates only on the node passed to it and potentially its local neighborhood. Specifically,
    /// it may not make any changes that invalidate the visitation loop currently iterating over the original model's nodes.
    /// It is permissible to alter previously-visited nodes in the output, though.
    class NodeLocalOptimizationPass : public OptimizationPass
    {
    public:
        /// <summary> Optimization method for this pass. Performs the optimization. </summary>
        ///
        /// <param name="node"> The current node being visited. </param>
        /// <param name="transformer"> The transformer object operating on the model. </param>
        virtual void OptimizeNode(const model::Node& node, model::ModelTransformer& transformer) = 0;

        void Initialize(ModelOptimizer& optimizer, const Model& model) override;
        Model Run(ModelOptimizer& optimizer, const Model& model) override;
        void Finalize(ModelOptimizer& optimizer, const Model& model) override;
    };
}
}
