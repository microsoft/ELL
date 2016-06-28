////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelGraph.h"
#include "Port.h"

// stl
#include <unordered_map>
#include <iostream>

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

    Model Model::Copy() const
    {
        std::unordered_map<const Node*, Node*> nodeMap;
        std::unordered_map<const Port*, Port*> portMap;

        Model newModel;
        Visit([&newModel, &portMap, &nodeMap](const Node& node)
        {
            node.Copy(newModel, nodeMap, portMap);
        });

        return newModel;
    }

    Model Model::Refine() const
    {
        std::unordered_map<const Node*, Node*> nodeMap;
        std::unordered_map<const Port*, Port*> portMap;

        Model newModel;
        Visit([&newModel, &portMap, &nodeMap](const Node& node)
        {
            node.Refine(newModel, nodeMap, portMap);
        });

        return newModel;
    }
}
