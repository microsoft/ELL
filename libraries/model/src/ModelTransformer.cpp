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
    Model ModelTransformer::CopyModel(const Model& oldModel, const TransformContext& context)
    {
        _context = context;
        _model = Model();
        _portToPortMap.clear();
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

            std::unordered_map<const Port*, Port*> currentPortToPortMap = std::move(_portToPortMap);
            _portToPortMap = std::unordered_map<const Port*, Port*>();

            _isModelCompilable = true;

            // one refinement pass
            currentModel.Visit([this](const Node& node) 
            { 
                node.Refine(*this); 
            });

            // concatenate new port map onto existing port map
            if(currentPortToPortMap.size() > 0)
            {
                std::unordered_map<const Port*, Port*> newPortToPortMap;
                for(const auto& entry : currentPortToPortMap)
                {
                    newPortToPortMap[entry.first] = _portToPortMap[entry.second];
                }
                _portToPortMap = newPortToPortMap;
            }

            // die after too many iterations
            if(++iterationCount >= maxRefinementIterations)
            {
                std::string uncompilableNodeName;
                auto uncompilableNode = GetUncompilableNode(currentModel, context);
                uncompilableNodeName = uncompilableNode->GetRuntimeTypeName();
                throw new utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "More than 10 refinement iterations, uncompilable node: " + uncompilableNodeName);
            }
        }
        while(!_isModelCompilable);

        _context = TransformContext();
        return _model;
    }

    const Port* ModelTransformer::GetCorrespondingPort(const Port& port)
    {
        if (_portToPortMap.find(&port) == _portToPortMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Could not find port in new model.");
        }
        return _portToPortMap[&port];
    }

    void ModelTransformer::MapPort(const Port& oldPort, const Port& newPort)
    {
        // this is hideous
        auto nonconstPort = const_cast<Port*>(&newPort);
        _portToPortMap[&oldPort] = nonconstPort;
    }

    const Node* ModelTransformer::GetUncompilableNode(const Model& model, const TransformContext& context) const
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
