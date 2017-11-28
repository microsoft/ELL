////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Model.h"
#include "InputPort.h"
#include "Node.h"
#include "Port.h"

// stl
#include <unordered_map>

namespace ell
{
namespace model
{
    namespace
    {
        //
        // Relevant archive format versions
        //
        constexpr utilities::ArchiveVersion noMetadataArchiveVersion = { utilities::ArchiveVersionNumbers::v2 };
        constexpr utilities::ArchiveVersion metadataArchiveVersion = { utilities::ArchiveVersionNumbers::v3_model_metadata };
    }

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

    const Node* Model::GetNode(Node::NodeId id) const
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

    utilities::ArchiveVersion Model::GetArchiveVersion() const
    {
        if (_metadata.IsEmpty())
        {
            return noMetadataArchiveVersion;
        }
        else
        {
            return metadataArchiveVersion;
        }
    }

    bool Model::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version >= noMetadataArchiveVersion && version <= metadataArchiveVersion;
    }

    void Model::WriteToArchive(utilities::Archiver& archiver) const
    {
        std::vector<const Node*> nodes;
        auto nodeIter = GetNodeIterator();
        while (nodeIter.IsValid())
        {
            nodes.push_back(nodeIter.Get());
            nodeIter.Next();
        }

        archiver["nodes"] << nodes;
        if (!_metadata.IsEmpty())
        {
            archiver["metadata"] << _metadata;
        }
    }

    void Model::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        ModelSerializationContext modelContext(archiver.GetContext(), this);
        archiver.PushContext(modelContext);

        // read nodes into big array
        std::vector<std::unique_ptr<Node>> nodes;
        archiver["nodes"] >> nodes;

        // Now add them to the model
        for (auto& node : nodes)
        {
            std::shared_ptr<Node> sharedNode = std::shared_ptr<Node>(node.release());
            sharedNode->RegisterDependencies();
            _idToNodeMap[sharedNode->GetId()] = sharedNode;
        }
        if (archiver.HasNextPropertyName("metadata"))
        {
            archiver["metadata"] >> _metadata;
        }
        archiver.PopContext();
    }

    void Model::Print(std::ostream& os) const
    {
        Visit([&os](const Node& node) { node.Print(os); });
    }

    void Model::PrintSubset(std::ostream& os, const Node* output) const
    {
        VisitSubset(output, [&os](const Node& node) { node.Print(os); });
    }

    //
    // NodeIterator implementation
    //

    NodeIterator::NodeIterator(const Model* model, const std::vector<const Node*>& outputNodes)
        : _model(model)
    {
        _currentNode = nullptr;
        if (_model->Size() == 0)
        {
            return;
        }

        // start with output nodes in the stack
        _stack = outputNodes;

        if (_stack.size() == 0) // Visit full model
        {
            // Just push everything on the stack
            for (auto node : _model->_idToNodeMap)
            {
                _stack.push_back(node.second.get());
            }
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
            const auto& inputPorts = node->GetInputPorts();
            for (auto inputPort : inputPorts)
            {
                for (const auto& parentNode : inputPort->GetParentNodes())
                {
                    canVisit = canVisit && _visitedNodes.find(parentNode) != _visitedNodes.end();
                }
            }

            if (canVisit)
            {
                _stack.pop_back();
                _visitedNodes.insert(node);
                _currentNode = node;
                break;
            }
            else // visit node's inputs
            {
                const auto& inputPorts = node->GetInputPorts();
                for (auto input : ModelImpl::Reverse(inputPorts)) // Visiting the inputs in reverse order more closely retains the order the nodes were originally created
                {
                    for (const auto& parentNode : input->GetParentNodes())
                    {
                        _stack.push_back(parentNode);
                    }
                }
            }
        }
    }

    //
    // ModelSerializationContext
    //
    ModelSerializationContext::ModelSerializationContext(utilities::SerializationContext& previousContext, const Model* model)
        : utilities::SerializationContext(previousContext, {}), _model(model)
    {
        auto mapContext = dynamic_cast<ModelSerializationContext*>(&previousContext);
        if (mapContext != nullptr)
        {
            mapContext->SetModel(model);
        }
    }

    void ModelSerializationContext::SetModel(const Model* model)
    {
        _model = model;
    }

    Node* ModelSerializationContext::GetNodeFromSerializedId(const Node::NodeId& id)
    {
        return _oldToNewNodeMap[id];
    }

    void ModelSerializationContext::MapNode(const Node::NodeId& id, Node* node)
    {
        _oldToNewNodeMap[id] = node;

        // if the old context is a ModelSerializationContext, forward the MapNode call to it
        auto mapContext = dynamic_cast<ModelSerializationContext*>(GetPreviousContext());
        if (mapContext != nullptr)
        {
            mapContext->MapNode(id, node);
        }
    }
}
}
