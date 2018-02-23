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

    std::vector<std::unique_ptr<OptimizationPass>>::iterator OptimizationPassList::begin()
    {
        return _passes.begin();
    }

    std::vector<std::unique_ptr<OptimizationPass>>::iterator OptimizationPassList::end()
    {
        return _passes.end();
    }

    //
    // ModelOptimizer
    //
    Model ModelOptimizer::OptimizeModel(const Model& model)
    {
        for(auto& pass: _passes)
        {
            pass->Initialize(*this, model);
        }

        TransformContext context;
        Model result = _transformer.CopyModel(model, context);
        for(auto& pass: _passes)
        {
            result = pass->Run(*this, result);
        }

        for(auto& pass: _passes)
        {
            pass->Finalize(*this, result);
        }
        return result;
    }

    void ModelOptimizer::AddPass(std::unique_ptr<OptimizationPass> pass)
    {
        _passes.AddPass(std::move(pass));
    }

    PortElementsBase ModelOptimizer::GetCorrespondingOutputs(const OutputPortBase& port)
    {
        return _transformer.GetCorrespondingOutputs(port);
    }

    PortElementsBase ModelOptimizer::GetCorrespondingOutputs(const PortElementsBase& elements)
    {
        return _transformer.GetCorrespondingOutputs(elements);
    }

    InputNodeBase* ModelOptimizer::GetCorrespondingInputNode(const InputNodeBase* node)
    {
        return _transformer.GetCorrespondingInputNode(node);
    }

    ModelTransformer& ModelOptimizer::GetTransformer()
    {
        return _transformer;
    }
}
}
