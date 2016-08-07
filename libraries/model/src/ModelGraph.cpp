////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelGraph.h"
#include "Port.h"
#include "Node.h"

// stl
#include <unordered_map>
#include <iostream>

/// <summary> model namespace </summary>
namespace model
{
    //
    // Model
    //
    Node* Model::GetNode(Node::NodeId id)
    {
        auto it = _idToNodeMap.find(id);
        if (it == _idToNodeMap.end())
        {
            return nullptr; // weak_ptr equivalent of nullptr
        }
        else
        {
            return it->second.get();
        }
    }

    NodeIterator Model::GetNodeIterator(const std::vector<const Node*>& outputNodes) const
    {
        return NodeIterator(this, outputNodes);
    }

    void Model::Serialize(utilities::Serializer& serializer) const
    {
        std::cout << "Serializing " << _idToNodeMap.size() << " nodes" << std::endl;
        std::vector<const Node*> nodes;
        auto nodeIter = GetNodeIterator();
        while(nodeIter.IsValid())
        {
            const auto& node = nodeIter.Get();
            std::cout << "Serializing node of type " << node->GetRuntimeTypeName() << std::endl;
            nodes.push_back(node);
            nodeIter.Next();
        }

        serializer.Serialize("nodes", nodes);
    }

    void Model::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) 
    {
        ModelSerializationContext graphContext(context, this);
        
        // Deserialize nodes into big array
        std::vector<std::unique_ptr<Node>> nodes;
        serializer.Deserialize("nodes", nodes, graphContext);

        // Now add them to the model graph
        for(auto& node: nodes)
        {
            std::shared_ptr<Node> sharedNode = std::shared_ptr<Node>(node.release());
            std::cout << "Fixing up node " << sharedNode->GetRuntimeTypeName() << ", Id: " << sharedNode->GetId() << std::endl;
            sharedNode->RegisterDependencies();
            _idToNodeMap[sharedNode->GetId()] = sharedNode;
        }
    }

    //
    // NodeIterator implementation
    //

    NodeIterator::NodeIterator(const Model* model, const std::vector<const Node*>& outputNodes) : _model(model)
    {
        _currentNode = nullptr;
        _visitFullGraph = false;
        if (_model->Size() == 0)
        {
            return;
        }

        // start with output nodes in the stack
        _stack = outputNodes;

        if (_stack.size() == 0) // Visit full graph
        {
            // helper function to find a terminal (output) node
            auto IsLeaf = [](const Node* node) { return node->GetDependentNodes().size() == 0; };

            // start with some arbitrary node
            auto iter = _model->_idToNodeMap.begin();
            const Node* anOutputNode = iter->second.get(); // !!! need private access

            // follow dependency chain until we get an output node
            while (!IsLeaf(anOutputNode))
            {
                anOutputNode = anOutputNode->GetDependentNodes()[0];
            }
            _stack.push_back(anOutputNode);
            _visitFullGraph = true;
        }

        Next();
    }

    void NodeIterator::Next()
    {
        _currentNode = nullptr;
        while (_stack.size() > 0)
        {
            const Node* node = _stack.back();

            // check if we've already visited this node
            if (_visitedNodes.find(node) != _visitedNodes.end())
            {
                _stack.pop_back();
                continue;
            }

            // we can visit this node only if all its inputs have been visited already
            bool canVisit = true;
            const auto& nodeInputs = node->GetInputPorts();
            std::cout << "# input ports for node " << node->GetId() << ": " << nodeInputs.size() << std::endl;
            for (auto input : nodeInputs)
            {
                std::cout << "Checking input port " << input->GetName() << " with "<< input->GetParentNodes().size() << " parent nodes" << std::endl;
                for (const auto& inputNode : input->GetParentNodes())
                {
                    std::cout << "Checking if we visited node " << inputNode->GetId() << std::endl;
                    canVisit = canVisit && _visitedNodes.find(inputNode) != _visitedNodes.end();
                }
            }

            if (canVisit)
            {
                _stack.pop_back();
                _visitedNodes.insert(node);

                // In "visit whole graph" mode, we want to add dependent nodes, so we can get to parts of the graph
                // that the original leaf node doesn't depend on
                if (_visitFullGraph)
                {

                    // now add all our children (Note: this part is the only difference between visit-all and visit-active-graph
                    const auto& dependentNodes = node->GetDependentNodes();
                    for (const auto& child : ModelImpl::Reverse(dependentNodes)) // Visiting the children in reverse order more closely retains the order the features were originally created
                    {
                        // note: this is kind of inefficient --- we're going to push multiple copies of child on the stack. But we'll check if we've visited it already when we pop it off.
                        // TODO: optimize this if it's a problem
                        _stack.push_back(child);
                    }
                }

                _currentNode = node;
                break;
            }
            else // visit node's inputs
            {
                const auto& nodeInputs = node->GetInputPorts();
                for (auto input : ModelImpl::Reverse(nodeInputs)) // Visiting the inputs in reverse order more closely retains the order the features were originally created
                {
                    for (const auto& inputNode : input->GetParentNodes())
                    {
                        _stack.push_back(inputNode);
                    }
                }
            }
        }
    }

    //
    // ModelSerializationContext
    //
    ModelSerializationContext::ModelSerializationContext(Model* model) : _originalContext(_dummyContext), _model(model) 
    {
    }
    
    ModelSerializationContext::ModelSerializationContext(utilities::SerializationContext& otherContext, Model* model) : _originalContext(otherContext), _model(model) 
    {
    }
    
    Node* ModelSerializationContext::GetNodeFromId(const Node::NodeId& id)
    {
        // TODO: error checking
        return _oldToNewNodeMap[id];
    }

    void ModelSerializationContext::MapNode(const Node::NodeId& id, Node* node)
    {
        // TODO: error checking
        _oldToNewNodeMap[id] = node;
    }
}
