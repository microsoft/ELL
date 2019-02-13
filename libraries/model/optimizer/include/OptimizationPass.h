////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationPass.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace model
{
    struct MapCompilerOptions;
    class Model;
    class ModelOptimizer;
    class ModelOptimizerContext;
    class ModelTransformer;
    class Node;

    /// <summary> Base class for optimization passes </summary>
    class OptimizationPass
    {
    public:
        virtual ~OptimizationPass();

        /// <summary> Perform any pre-optimization initialization required by the pass. </summary>
        /// This method is always called before the optimization pass is started.
        ///
        /// <param name="model"> The model about to be optimized. </param>
        /// <param name="settings"> The compiler settings for the model being optimized. </param>
        /// <param name="context"> The optimization context object for this run of the optimizer. </param>
        virtual void Initialize(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& context) const;

        /// <summary> Run this pass. </summary>
        ///
        /// <param name="model"> The model being optimized. </param>
        /// <param name="settings"> The compiler settings for the model being optimized. </param>
        /// <param name="context"> The optimization context object for this run of the optimizer. </param>
        virtual Model Run(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& context) const = 0;

        /// <summary> Perform any post-optimization teardown required by the pass. </summary>
        /// This method is always called after the optimization pass is finished.
        ///
        /// <param name="model"> The (new) model that was the result of the optimization. </param>
        /// <param name="settings"> The compiler settings for the model being optimized. </param>
        /// <param name="context"> The optimization context object for this run of the optimizer. </param>
        virtual void Finalize(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& context) const;
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
        /// <param name="settings"> The compiler settings for the model being optimized. </param>
        /// <param name="context"> The optimization context object for this run of the optimizer. </param>
        virtual void OptimizeNode(const model::Node& node, const MapCompilerOptions& settings, ModelOptimizerContext& context) const = 0;

        /// <summary> Run this pass. </summary>
        ///
        /// <param name="model"> The model being optimized. </param>
        /// <param name="settings"> The compiler settings for the model being optimized. </param>
        /// <param name="context"> The optimization context object for this run of the optimizer. </param>
        Model Run(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& context) const final;
    };
} // namespace model
} // namespace ell
