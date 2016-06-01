#include "Node.h"
#include "Edge.h"

//
// Port
//
size_t Node::Port::GetSize()
{
    return _size;
}

Node::Port::Type Node::Port::GetType()
{
    return _type;
}



//
// Node
//
Node::Node()
{}

Node::Node(const std::vector<Edge>& inputs) : _inputs(inputs)
{
    for(const auto& edge: inputs)
    {
        _dependents.push_back(edge.GetNode());
    }
}

void Node::AddDependent(const std::shared_ptr<Node>& dependent) 
{ 
    _dependents.push_back(dependent); 
}
