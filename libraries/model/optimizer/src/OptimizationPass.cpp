////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizationPass.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizationPass.h"

// model
#include "Model.h"
#include "ModelOptimizer.h"
#include "ModelTransformer.h"

namespace ell
{
namespace model
{
    //
    // NodeLocalOptimizationPass
    //
    void NodeLocalOptimizationPass::Initialize(ModelOptimizer& optimizer, const Model& model)
    {
        // nothing
    }

    Model NodeLocalOptimizationPass::Run(ModelOptimizer& optimizer, const Model& model)
    {
        model::TransformContext context;
        auto& transformer = optimizer.GetTransformer();
        return transformer.TransformModel(model, context, [this](const model::Node& node, model::ModelTransformer& transformer) {
            this->OptimizeNode(node, transformer);
        });
    }

    void NodeLocalOptimizationPass::Finalize(ModelOptimizer& optimizer, const Model& model)
    {
        // nothing
    }
}
}
