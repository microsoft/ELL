////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationPass.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizationPass.h"

#include <model/include/MapCompilerOptions.h>
#include <model/include/Model.h>
#include "ModelOptimizer.h"
#include <model/include/ModelTransformer.h>

#include <utilities/include/Unused.h>

#include <cassert>

namespace ell
{
namespace model
{
    //
    // OptimizationPass
    //
    OptimizationPass::~OptimizationPass() = default;

    void OptimizationPass::Initialize(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& context) const
    {
        UNUSED(model, settings, context);
    }

    void OptimizationPass::Finalize(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& context) const
    {
        UNUSED(model, settings, context);
    }

    //
    // NodeLocalOptimizationPass
    //
    Model NodeLocalOptimizationPass::Run(const Model& model, const MapCompilerOptions& settings, ModelOptimizerContext& optimizerContext) const
    {
        model::TransformContext transformContext;
        return optimizerContext.GetTransformer().TransformModel(model, transformContext, [this, &settings, &optimizerContext](const model::Node& node, model::ModelTransformer& transformer) {
            // The transformer that gets passed in to the lambda had better be the same one that's in the context.
            // This will get fixed in a future redesign
            assert(&transformer == &(optimizerContext.GetTransformer()));
            this->OptimizeNode(node, settings, optimizerContext);
        });
    }
} // namespace model
} // namespace ell
