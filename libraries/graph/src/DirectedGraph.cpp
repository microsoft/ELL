#include "DirectedGraph.h"

std::weak_ptr<Node> DirectedGraph::GetNode(Node::NodeId id)
{
    auto it = _nodeMap.find(id);
    if(it == _nodeMap.end())
    {
        return std::weak_ptr<Node>();
    }
    else
    {
        return it->second;
    }
}