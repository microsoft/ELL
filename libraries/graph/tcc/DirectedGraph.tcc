//
// DirectedGraph.tcc
//

#include <unordered_set>

//
// Factory method
//
template <typename NodeType, typename... Args>
std::shared_ptr<NodeType> DirectedGraph::AddNode(Args... args)
{
    // TODO: Store node somewhere
    auto result = std::make_shared<NodeType>(args...);
    result->AddDependents();
    
    // _nodeMap[result->Id()] = result;    
    return result;
}

//
// Visitors
//
template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor) const
{
    // TODO: this    
}

template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor, const std::shared_ptr<Node>& outputNode) const
{
    std::vector<std::shared_ptr<Node>> x = {outputNode};
    Visit(visitor, x);
}

template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor, const std::vector<std::shared_ptr<Node>>& outputNodes) const
{
    std::unordered_set<const Node*> visitedNodes;

    // start with output nodes in the stack
    std::vector<const Node*> stack;
    for(auto outputNode: outputNodes)
    {
        stack.push_back(outputNode.get());
    }
    
    while (stack.size() > 0)
    {
        const Node* node = stack.back();

        // check if we've already visited this node
        if (visitedNodes.find(node) != visitedNodes.end())
        {
            stack.pop_back();
            continue;
        }

        // we can visit this node only if all its inputs have been visited already
        bool canVisit = true;
        for (auto input : node->_inputs)
        {
            auto inputNode = input->GetNode();
            canVisit = canVisit && visitedNodes.find(inputNode) != visitedNodes.end();
        }

        if (canVisit)
        {
            stack.pop_back();
            visitedNodes.insert(node);
            
            visitor(*node);
        }
        else // visit node's inputs
        {
            for (auto input : node->_inputs)
            {
                stack.push_back(input->GetNode());
            }
        }
    }    
}

