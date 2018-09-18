////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelOptimizer.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelOptimizer.h"
#include "OptimizationPass.h"

// model
#include "ModelTransformer.h"

// utilities
#include "Exception.h"

namespace ell
{
namespace model
{
    //
    // OptimizationPassList
    //
    OptimizationPassList::~OptimizationPassList() = default;

    void OptimizationPassList::AddPass(std::unique_ptr<OptimizationPass> pass)
    {
        _passes.emplace_back(std::move(pass));
    }

    OptimizationPassList::InternalListType::iterator OptimizationPassList::begin()
    {
        return _passes.begin();
    }

    OptimizationPassList::InternalListType::iterator OptimizationPassList::end()
    {
        return _passes.end();
    }

    OptimizationPassList::InternalListType::const_iterator OptimizationPassList::begin() const
    {
        return _passes.cbegin();
    }

    OptimizationPassList::InternalListType::const_iterator OptimizationPassList::end() const
    {
        return _passes.cend();
    }

    //
    // ModelOptimizerContext
    //

    ModelTransformer& ModelOptimizerContext::GetTransformer()
    {
        return _transformer;
    }

    const OutputPortBase& ModelOptimizerContext::GetCorrespondingOutputs(const OutputPortBase& port)
    {
        return _transformer.GetCorrespondingOutputs(port);
    }

    const OutputPortBase& ModelOptimizerContext::GetCorrespondingOutputs(const PortElementsBase& elements)
    {
        return _transformer.GetCorrespondingOutputs(elements);
    }

    InputNodeBase* ModelOptimizerContext::GetCorrespondingInputNode(const InputNodeBase* node)
    {
        return _transformer.GetCorrespondingInputNode(node);
    }

    //
    // ModelOptimizer
    //
    ModelOptimizer::ModelOptimizer(const MapCompilerOptions& settings)
        : _settings(settings)
    {
    }

    Model ModelOptimizer::OptimizeModel(const Model& model, ModelOptimizerContext& context) const
    {
        context.GetTransformer().Reset();
        TransformContext transformContext;
        Model result = context.GetTransformer().CopyModel(model, transformContext);

        for (auto& pass : _passes)
        {
            pass->Initialize(result, _settings, context);
        }

        for (auto& pass : _passes)
        {
            result = pass->Run(result, _settings, context);
        }

        for (auto& pass : _passes)
        {
            pass->Finalize(result, _settings, context);
        }
        return result;
    }

    void ModelOptimizer::AddPass(std::unique_ptr<OptimizationPass> pass)
    {
        _passes.AddPass(std::move(pass));
    }
}
}
