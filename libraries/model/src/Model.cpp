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
#include "SliceNode.h"
#include "SpliceNode.h"

// utility
#include "StringUtil.h"

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

        //
        // Utility functions
        //
        template <typename ContainerType>
        class ReverseRange
        {
        public:
            ReverseRange(const ContainerType& container)
                : _begin(container.crbegin()), _end(container.crend()) {}

            auto begin() const { return _begin; }

            auto end() const { return _end; }

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
    // Model
    //

    Model::Model() : _data(std::make_shared<Model::ModelData>())
    {
    }

    Model::Model(const std::shared_ptr<Model::ModelData>& data) : _data(data)
    {
    }

    Model Model::ShallowCopy() const
    {
        return { _data };
    }

    bool Model::NodeIdExists(Node::NodeId id) const
    {
        return _data->idToNodeMap.find(id) != _data->idToNodeMap.end();
    }

    Node* Model::GetNode(Node::NodeId id)
    {
        auto it = _data->idToNodeMap.find(id);
        if (it == _data->idToNodeMap.end())
        {
            return nullptr;
        }
        else
        {
            return it->second.get();
        }
    }

    const Node* Model::GetNode(Node::NodeId id) const
    {
        auto it = _data->idToNodeMap.find(id);
        if (it == _data->idToNodeMap.end())
        {
            return nullptr;
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
        if (_data->metadata.IsEmpty())
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
        if (!_data->metadata.IsEmpty())
        {
            archiver["metadata"] << _data->metadata;
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
            AddExistingNode(std::move(node));
        }
        if (archiver.HasNextPropertyName("metadata"))
        {
            archiver["metadata"] >> _data->metadata;
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

    void Model::Reset()
    {
        auto reset = [](const Node& node) { const_cast<Node&>(node).Reset(); };
        Visit(reset);
    }

    Node* Model::AddExistingNode(std::unique_ptr<Node> node)
    {
        std::shared_ptr<Node> sharedNode(std::move(node));
        EnsureNodeHasUniqueId(*sharedNode);
        sharedNode->RegisterDependencies();
        _data->idToNodeMap[sharedNode->GetId()] = sharedNode;
        return sharedNode.get();
    }

    void Model::EnsureNodeHasUniqueId(Node& node)
    {
        if (NodeIdExists(node.GetId()))
        {
            node.SetId(GetUniqueId(node.GetId()));
        }
    }

    Node::NodeId Model::GetUniqueId(const Node::NodeId& desiredId)
    {
        Node::NodeId currentId = desiredId;
        while (NodeIdExists(currentId))
        {
            currentId = GetNextId(currentId);
        }
        return currentId;
    }

    Node::NodeId Model::GetNextId(Node::NodeId id)
    {
        auto substrings = utilities::Split(id.ToString(), '_');
        if (substrings.size() == 1 || substrings.back().find_first_not_of("0123456789") != std::string::npos)
        {
            substrings.push_back("_1");
        }
        else
        {
            int nextIndex = std::stoi(substrings.back()) + 1;
            substrings.push_back(std::string("_") + std::to_string(nextIndex));
        }

        return Node::NodeId(utilities::Join(substrings, "_"));
    }
    
    PortElementsBase Model::AddRoutingNodes(const PortElementsBase& elements)
    {
        const auto numRanges = elements.NumRanges();
        if (numRanges == 0)
        {
            return PortElementsBase{ elements };
        }

        // get vector of output ports to concatenate
        std::vector<const OutputPortBase*> concatInputPorts;
        for(const auto& range: elements.GetRanges())
        {
            if (range.IsFullPortRange())
            {
                concatInputPorts.push_back(range.ReferencedPort());
            }
            else
            {
                // Add a SliceNode
                auto portRange = AddPortRange(range);
                concatInputPorts.push_back(portRange);
            }
        }

        if (numRanges > 1)
        {
            auto concatNodeOutput = AddConcat(concatInputPorts);
            return { *concatNodeOutput };
        }
        else
        {
            return PortElementsBase{ *concatInputPorts[0] };
        }
    }

    const OutputPortBase* Model::AddPortRange(const PortRange& inputRange)
    {
        auto port = inputRange.ReferencedPort();
        auto layout = port->GetMemoryLayout();
        auto increment = layout.GetCumulativeIncrement(0); // slowest-moving dimension
        if (inputRange.GetStartIndex() % increment != 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SliceNode input start location must be multiple of largest dimension increment");
        }

        if (inputRange.Size() % increment != 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SliceNode input count must be multiple of largest dimension increment");
        }
        auto start = inputRange.GetStartIndex() / increment;
        auto count = inputRange.Size() / increment;
        switch (port->GetType())
        {
        case Port::PortType::boolean:
            return &(AddNode<SliceNode<bool>>(port, start, count)->output);
            break;
        case Port::PortType::integer:
            return &(AddNode<SliceNode<int>>(port, start, count)->output);
            break;
        case Port::PortType::bigInt:
            return &(AddNode<SliceNode<int64_t>>(port, start, count)->output);
            break;
        case Port::PortType::smallReal:
            return &(AddNode<SliceNode<float>>(port, start, count)->output);
            break;
        case Port::PortType::real:
            return &(AddNode<SliceNode<double>>(port, start, count)->output);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    const OutputPortBase* Model::AddConcat(const std::vector<const OutputPortBase*>& outputPorts)
    {
        auto portType = outputPorts[0]->GetType();
        switch (portType)
        {
        case Port::PortType::boolean:
            return &(AddNode<SpliceNode<bool>>(outputPorts)->output);
            break;
        case Port::PortType::integer:
            return &(AddNode<SpliceNode<int>>(outputPorts)->output);
            break;
        case Port::PortType::bigInt:
            return &(AddNode<SpliceNode<int64_t>>(outputPorts)->output);
            break;
        case Port::PortType::smallReal:
            return &(AddNode<SpliceNode<float>>(outputPorts)->output);
            break;
        case Port::PortType::real:
            return &(AddNode<SpliceNode<double>>(outputPorts)->output);
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
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

        if (_stack.empty()) // Visit full model
        {
            // Just push everything on the stack
            for (auto node : _model->_data->idToNodeMap)
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
                for (auto input : Reverse(inputPorts)) // Visiting the inputs in reverse order more closely retains the order the nodes were originally created
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
    ModelSerializationContext::ModelSerializationContext(utilities::SerializationContext& previousContext, Model* model)
        : utilities::SerializationContext(previousContext, {}), _model(model)
    {
        auto mapContext = dynamic_cast<ModelSerializationContext*>(&previousContext);
        if (mapContext != nullptr)
        {
            mapContext->SetModel(model);
        }
    }

    void ModelSerializationContext::SetModel(Model* model)
    {
        _model = model;
    }

    Node* ModelSerializationContext::GetNodeFromSerializedId(const Node::NodeId& id) const
    {
        return _oldToNewNodeMap.at(id);
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

