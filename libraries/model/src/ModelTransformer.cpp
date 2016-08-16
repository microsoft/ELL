////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelTransformer.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTransformer.h"
#include "Node.h"

// utilities
#include "Exception.h"

/// <summary> model namespace </summary>
namespace model
{
    //
    // TransformContext implementation
    //
    TransformContext::TransformContext()
    {
        _isNodeCompilableFunction = [](const Node& node) { return false; };
    }

    TransformContext::TransformContext(const std::function<bool(const Node&)>& isNodeCompilable) : _isNodeCompilableFunction(isNodeCompilable)
    {}

    bool TransformContext::IsNodeCompilable(const Node& node) const
    {
        return _isNodeCompilableFunction(node);
    }

    //
    // ModelTransformer implementation
    //
    Model ModelTransformer::CopyModel(const Model& oldModel, const TransformContext& context)
    {
        _context = context;
        _model = Model();
        _elementToElementMap.clear();
        oldModel.Visit([this](const Node& node) { node.Copy(*this); });
        _context = TransformContext();

        return _model;
    }

    Model ModelTransformer::RefineModel(const Model& oldModel, const TransformContext& context)
    {
        _context = context;
        _model = oldModel;

        int iterationCount = 0;

        // refine until all nodes are compilable according to context.IsNodeCompilable()
        do
        {
            Model currentModel = std::move(_model);
            _model = Model();

            auto currentElementToElementMap = std::move(_elementToElementMap);
            _elementToElementMap.clear();

            _isModelCompilable = true;

            // one refinement pass
            bool didRefineAny = false;
            currentModel.Visit([this, &didRefineAny](const Node& node) 
            { 
                bool didRefineNode = node.Refine(*this); 
                didRefineAny |= didRefineNode;
            });

            // concatenate new port map onto existing port map
            if(currentElementToElementMap.size() > 0)
            {
                std::unordered_map<PortElementBase, PortElementBase> newElementToElementMap;
                for(const auto& entry : currentElementToElementMap)
                {
                    newElementToElementMap[entry.first] = _elementToElementMap[entry.second];
                }
                _elementToElementMap = newElementToElementMap;
            }

            // return if we didn't make any progress 
            if(!didRefineAny)
            {
                break;
            }

            // die after too many iterations
            if(++iterationCount >= maxRefinementIterations)
            {
                std::string uncompilableNodeName;
                auto uncompilableNode = FindFirstUncompilableNode(currentModel, context);
                uncompilableNodeName = uncompilableNode->GetRuntimeTypeName();
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "More than " + std::to_string(maxRefinementIterations) + " refinement iterations, uncompilable node: " + uncompilableNodeName);
            }
        }
        while(!_isModelCompilable);

        // clear out the context
        _context = TransformContext();
        return _model;
    }

    const Node* ModelTransformer::FindFirstUncompilableNode(const Model& model, const TransformContext& context) const
    {
        auto iter = model.GetNodeIterator();
        while(iter.IsValid())
        {
            auto node = iter.Get();
            if(!context.IsNodeCompilable(*node))
            {
                return node;
            }
            iter.Next();
        }
        return nullptr;
    }
}
