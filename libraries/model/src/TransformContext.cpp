////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformContext.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformContext.h"
#include "InputNode.h"
#include "Node.h"
#include "OutputNode.h"
#include "RefineTransformation.h"

#include <utilities/include/Exception.h>
#include <utilities/include/StringUtil.h>

#include <algorithm>

namespace ell
{
namespace model
{
    TransformContext::TransformContext() :
        _compiler(nullptr)
    {
    }

    TransformContext::TransformContext(const NodeActionFunction& nodeActionFunction) :
        _compiler(nullptr)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    TransformContext::TransformContext(const MapCompiler* compiler) :
        _compiler(compiler)
    {
    }

    TransformContext::TransformContext(const MapCompiler* compiler, const NodeActionFunction& nodeActionFunction) :
        _compiler(compiler)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    bool TransformContext::IsNodeCompilable(const Node& node) const
    {
        return node.IsCompilable(_compiler);
    }

    void TransformContext::AddNodeActionFunction(const NodeActionFunction& nodeActionFunction)
    {
        _nodeActionFunctions.emplace_back(nodeActionFunction);
    }

    NodeAction TransformContext::GetNodeAction(const Node& node) const
    {
        for (auto iter = _nodeActionFunctions.rbegin(); iter != _nodeActionFunctions.rend(); ++iter)
        {
            auto& actionFunction = *iter;
            auto action = actionFunction(node);
            if (action != NodeAction::abstain)
            {
                return action;
            }
        }
        return node.IsCompilable(_compiler) ? NodeAction::compile : NodeAction::refine;
    }
} // namespace model
} // namespace ell
