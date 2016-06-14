////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelGraph.h"

/// <summary> model namespace </summary>
namespace model
{
    std::weak_ptr<Node> Model::GetNode(Node::NodeId id)
    {
        auto it = _nodeMap.find(id);
        if (it == _nodeMap.end())
        {
            return std::weak_ptr<Node>(); // weak_ptr equivalent of nullptr
        }
        else
        {
            return it->second;
        }
}
}
