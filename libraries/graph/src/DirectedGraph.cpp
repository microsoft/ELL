#include "DirectedGraph.h"
#include "Node.h"

#include <ostream>

template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor) const
{
    
}

template <typename Visitor>
void DirectedGraph::Visit(const NodeRef outputNode, Visitor& visitor) const
{
    
}

template <typename Visitor>
void DirectedGraph::Visit(const std::vector<NodeRef>& outputNode, Visitor& visitor) const
{
    
}

