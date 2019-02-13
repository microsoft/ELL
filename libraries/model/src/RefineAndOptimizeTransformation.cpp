////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RefineAndOptimizeTransformation.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RefineAndOptimizeTransformation.h"
#include "OptimizeModelTransformation.h"
#include "RefineTransformation.h"

#include <model/include/ModelTransformer.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Logger.h>

namespace ell
{
namespace model
{
    namespace
    {
        bool IsConvolutionalLayerNode(const Node& node)
        {
            return (node.GetRuntimeTypeName().find("ConvolutionalLayerNode") == 0);
        }
    } // namespace

    using namespace logging;

    RefineAndOptimizeTransformation::RefineAndOptimizeTransformation(const ModelOptimizer& optimizer, int maxIterations) :
        _optimizer(optimizer),
        _maxIterations(maxIterations)
    {
    }

    Submodel RefineAndOptimizeTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        auto compiler = context.GetCompiler();
        auto noRefineConvLayerNodesFn = [compiler](const model::Node& node) {
            return IsConvolutionalLayerNode(node) || node.IsCompilable(compiler) ? model::NodeAction::compile : model::NodeAction::refine;
        };

        model::TransformContext noRefineConvLayerNodesContext{ compiler, noRefineConvLayerNodesFn };
        OptimizeModelTransformation optimizeTransformation(_optimizer);
        RefineTransformation refineTransformation(_maxIterations);

        Log() << "Refining the model..." << EOL;
        auto result = refineTransformation.Transform(submodel, transformer, noRefineConvLayerNodesContext);

        Log() << "Optimizing the model..." << EOL;
        result = optimizeTransformation.Transform(result, transformer, context);

        Log() << "Refining the model again..." << EOL;
        result = refineTransformation.Transform(result, transformer, context);

        Log() << "Optimizing the model again..." << EOL;
        result = optimizeTransformation.Transform(result, transformer, context);
        return result;
    }
} // namespace model
} // namespace ell
