//
// DirectedGraph.tcc
//

#include "NodeInput.h"

#include <unordered_set>


namespace
{
    // TODO: put this in some utility place
    template <typename ContainerType>
    class ReverseRange
    {
    public:
        ReverseRange(const ContainerType& container) : _begin(container.crbegin()), _end(container.crend()) {}
        typename ContainerType::const_reverse_iterator begin() const { return _begin; }
        typename ContainerType::const_reverse_iterator end() const { return _end; }
    
    private:
        typename ContainerType::const_reverse_iterator _begin;
        typename ContainerType::const_reverse_iterator _end;    
    };
    
    template <typename ContainerType>
    ReverseRange<ContainerType> Reverse(const ContainerType& container)
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
    _nodeMap[result->Id()] = result;
    result->AddDependents();
    return result;
}

//
// Compute output value
//
template <typename ValueType>
std::vector<ValueType> DirectedGraph::GetNodeOutput(const std::shared_ptr<Node>& outputNode, size_t outputIndex) const
{
    outputNode->ComputeOutput();
    return outputNode->GetOutputValue<ValueType>(outputIndex);
}

//
// Visitors
//

// Visits the entire graph
// TODO: merge code for this and active-graph visitor
template <typename Visitor>
void DirectedGraph::Visit(Visitor& visitor) const
{
    if(_nodeMap.size() == 0)
    {
        return;
    }

    // helper function to find a terminal node
    auto IsLeaf = [](const Node* node) { return node->GetDependents().size() == 0; };
    
    std::unordered_set<const Node*> visitedNodes;
    std::vector<const Node*> stack;

    // start with some arbitrary node
    const Node* anOutputNode = _nodeMap.begin()->second.get();
    // follow dependency chain until we get an output node
    while(!IsLeaf(anOutputNode))
    {
        anOutputNode = anOutputNode->GetDependents()[0];
    } 
    
    stack.push_back(anOutputNode);
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
            if(node != anOutputNode)
            {
                visitor(*node);               
            }
            
            // now add all our children (Note: this part is the only difference between visit-all and visit-active-graph
            for(const auto& child: Reverse(node->_dependentNodes)) // Visiting the children in reverse order more closely retains the order the features were originally created
            {
                // note: this is kind of inefficient --- we're going to push multiple copies of child on the stack. But we'll check if we've visited it already when we pop it off. 
                // TODO: optimize this if it's a problem
                stack.push_back(child);
            }
        }
        else // visit node's inputs
        {
            for (auto input : Reverse(node->_inputs)) // Visiting the inputs in reverse order more closely retains the order the features were originally created
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
        for (auto input: node->_inputs)
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
            for (auto input: Reverse(node->_inputs)) // Visiting the inputs in reverse order more closely retains the order the features were originally created
            {
                stack.push_back(input->GetNode()); // Again, if `NodeInput`s point to multiple nodes, need to iterate here
            }
        }
    }    
}

