//
// DirectedGraph.tcc
//

#include <unordered_set>


namespace
{
    // TODO: put this in some utility place
    template <typename ContainerType>
    class ReverseRange
    {
    public:
        ReverseRange(ContainerType& container) : _begin(container.rbegin()), _end(container.rend()) {}
        typename ContainerType::reverse_iterator begin() const { return _begin; }
        typename ContainerType::reverse_iterator end() const { return _end; }
    
    private:
        typename ContainerType::reverse_iterator _begin;
        typename ContainerType::reverse_iterator _end;    
    };
    
    template <typename ContainerType>
    ReverseRange<ContainerType> Reverse(ContainerType& container)
    {
        return ReverseRange<ContainerType>(container);
    }
}

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

// Visits the entire graph
template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor) const
{
    std::unordered_set<const Node*> visitedNodes;
    std::vector<const Node*> stack;

    // TODO: get some arbitrary node
    std::shared_ptr<Node> someArbitraryNode;
    std::shared_ptr<Node> anOutputNode; // TODO: follow dependency chain until we get an output node
    stack.push_back(anOutputNode.get());
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
            // Note: If NodeInputs can point to multiple nodes, we'll have to iterate over them here
            auto inputNode = input->GetNode();
            canVisit = canVisit && visitedNodes.find(inputNode) != visitedNodes.end();
        }

        if (canVisit)
        {
            stack.pop_back();
            visitedNodes.insert(node);
            
            // explicitly skip output feature (until we visit it manually at the end)
            if(node != anOutputNode.get())
            {
                visitor(*node);               
            }
            
            // now add children (Note: this part is the only difference between Visit and VisitActiveGraph)
            for(const auto& child: Reverse(node->_inputs)) // Visiting the children in reverse order more closely retains the order the features were originally created
            {
                // note: this is kind of inefficient --- we're going to push multiple copies of child on the stack. But we'll check if we've visited it already when we pop it off. 
                // TODO: optimize this if it's a problem
                auto childNode = child->GetNode();
                stack.push_back(childNode);
            }
        }
        else // visit node's inputs
        {
            for (auto input : node->_inputs) // TODO: should be for input: Reversed(node->_inputs)
            {
                stack.push_back(input->GetNode()); // Again, if `NodeInput`s point to multiple nodes, need to iterate here
            }
        }
    }
    visitor(*anOutputNode);
}


// Visits just the parts necessary to compute output
template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor, const std::shared_ptr<Node>& outputNode) const
{
    std::vector<std::shared_ptr<Node>> x = {outputNode};
    Visit(visitor, x);
}

// Visits just the parts necessary to compute outputs
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
            // Note: If NodeInputs can point to multiple nodes, we'll have to iterate over them here
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
                stack.push_back(input->GetNode()); // Again, if `NodeInput`s point to multiple nodes, need to iterate here
            }
        }
    }    
}

