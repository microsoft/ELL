////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetConvolutionMethodPass.h (passes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Model.h>

#include <model/optimizer/include/ModelOptimizer.h>
#include <model/optimizer/include/OptimizationPass.h>

#include <vector>

namespace ell
{
namespace passes
{
    /// <summary> An optimization pass that sets the convolution algorithm for a `ConvolutionalLayerNode`. </summary>
    class SetConvolutionMethodPass : public model::NodeLocalOptimizationPass
    {
    public:
        /// <summary> Set the convolution algorithm for a `ConvolutionalLayerNode` if possible. </summary>
        ///
        /// <param name="node"> The current node being visited. </param>
        /// <param name="transformer"> The transformer object operating on the model. </param>
        void OptimizeNode(const model::Node& node, const model::MapCompilerOptions& settings, model::ModelOptimizerContext& context) const override;

        /// <summary> Add this pass type to the global pass registry. </summary>
        static void AddToRegistry();
    };
} // namespace passes
} // namespace ell
