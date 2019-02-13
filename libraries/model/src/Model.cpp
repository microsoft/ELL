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

#include <utilities/include/StringUtil.h>

#include <algorithm>
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
            ReverseRange(const ContainerType& container) :
                _begin(container.crbegin()),
                _end(container.crend()) {}

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

        void VerifyPortsOfSameType(const std::vector<const OutputPortBase*>& outputPorts)
        {
            if (outputPorts.empty())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input ports list must not be empty");
            }

            if (outputPorts[0] == nullptr)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Input port must not be null");
            }

            auto portType = outputPorts[0]->GetType();
            std::for_each(outputPorts.begin() + 1, outputPorts.end(), [portType](const OutputPortBase* port) {
                if (port == nullptr)
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Input port must not be null");
                }
                else if (port->GetType() != portType)
                {
                    throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input ports must all be of the same type");
                }
            });
        }
    } // namespace

    //
    // Model
    //

    Model::Model() :
        _data(std::make_shared<Model::ModelData>())
    {
    }

    Model::Model(const std::shared_ptr<Model::ModelData>& data) :
        _data(data)
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

    ForwardNodeIterator Model::GetNodeIterator() const
    {
        return GetNodeIterator(std::vector<const OutputPortBase*>{});
    }

    ForwardNodeIterator Model::GetNodeIterator(const OutputPortBase* output) const
    {
        return GetNodeIterator(std::vector<const OutputPortBase*>{ output });
    }

    ForwardNodeIterator Model::GetNodeIterator(const std::vector<const OutputPortBase*>& outputs) const
    {
        return ForwardNodeIterator(this, outputs);
    }

    ForwardNodeIterator Model::GetNodeIterator(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) const
    {
        return ForwardNodeIterator(this, inputs, outputs);
    }

    ReverseNodeIterator Model::GetReverseNodeIterator() const
    {
        return GetReverseNodeIterator(std::vector<const OutputPortBase*>{});
    }

    ReverseNodeIterator Model::GetReverseNodeIterator(const OutputPortBase* output) const
    {
        return GetReverseNodeIterator(std::vector<const OutputPortBase*>{ output });
    }

    ReverseNodeIterator Model::GetReverseNodeIterator(const std::vector<const OutputPortBase*>& outputs) const
    {
        return ReverseNodeIterator(this, outputs);
    }

    ReverseNodeIterator Model::GetReverseNodeIterator(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) const
    {
        return ReverseNodeIterator(this, inputs, outputs);
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

    void Model::PrintSubset(std::ostream& os, const OutputPortBase* output) const
    {
        VisitSubmodel(output, [&os](const Node& node) { node.Print(os); });
    }

    bool Model::operator==(const Model& other) const
    {
        return _data == other._data;
    }

    bool Model::operator!=(const Model& other) const
    {
        return _data != other._data;
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
        sharedNode->SetModel(this);
        sharedNode->UpdateInputPorts();
        VerifyInputs(*sharedNode);
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

    void Model::Verify() const
    {
        VerifyNodes();
        VerifyInputs();
    }
    
    void Model::VerifyNodes() const
    {
        for (auto it: _data->idToNodeMap)
        {
            const Model* otherModel = it.second->GetModel();
            if ((*otherModel) != (*this))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model input validation error: nodes come from a different model");
            }
        }

        Visit([this](const Node& node) {
            const Model* otherModel = node.GetModel();
            if ((*otherModel) != (*this))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model input validation error: nodes come from a different model");
            }
        });
    }

    void Model::VerifyInputs() const
    {
        Visit([this](const Node& node) {
            VerifyInputs(node);
        });
    }

    void Model::VerifyInputs(const Node& node) const
    {
        for (auto it : node.GetInputPorts())
        {
            const Model* otherModel = it->GetReferencedPort().GetNode()->GetModel();
            if ((*otherModel) != (*this))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Model input validation error: inputs come from a different model");
            }
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

    const Model::IDToNodeMap& Model::GetNodeMap() const
    {
        return _data->idToNodeMap;
    }

    const OutputPortBase& Model::SimplifyOutputs(const PortElementsBase& elements)
    {
        const auto numRanges = elements.NumRanges();
        if (numRanges == 0)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Empty range.");
        }

        std::vector<const OutputPortBase*> inputsToConcatenate;
        for (const auto& range : elements.GetRanges())
        {
            if (range.IsFullPortRange())
            {
                inputsToConcatenate.push_back(range.ReferencedPort());
            }
            else
            {
                // Add a SliceNode
                const auto& portRange = AddSliceNode(range);
                inputsToConcatenate.push_back(&portRange);
            }
        }

        if (numRanges > 1)
        {
            // Add a SpliceNode
            const auto& concatNodeOutput = AddSpliceNode(inputsToConcatenate);
            return concatNodeOutput;
        }
        else
        {
            return *inputsToConcatenate[0];
        }
    }

    const OutputPortBase& Model::AddSliceNode(const PortRange& inputRange)
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
            return AddNode<SliceNode<bool>>(*port, start, count)->output;
            break;
        case Port::PortType::integer:
            return AddNode<SliceNode<int>>(*port, start, count)->output;
            break;
        case Port::PortType::bigInt:
            return AddNode<SliceNode<int64_t>>(*port, start, count)->output;
            break;
        case Port::PortType::smallReal:
            return AddNode<SliceNode<float>>(*port, start, count)->output;
            break;
        case Port::PortType::real:
            return AddNode<SliceNode<double>>(*port, start, count)->output;
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    const OutputPortBase& Model::AddSpliceNode(const std::vector<const OutputPortBase*>& outputPorts)
    {
        VerifyPortsOfSameType(outputPorts);

        auto portType = outputPorts[0]->GetType();
        switch (portType)
        {
        case Port::PortType::boolean:
            return AddNode<SpliceNode<bool>>(outputPorts)->output;
            break;
        case Port::PortType::integer:
            return AddNode<SpliceNode<int>>(outputPorts)->output;
            break;
        case Port::PortType::bigInt:
            return AddNode<SpliceNode<int64_t>>(outputPorts)->output;
            break;
        case Port::PortType::smallReal:
            return AddNode<SpliceNode<float>>(outputPorts)->output;
            break;
        case Port::PortType::real:
            return AddNode<SpliceNode<double>>(outputPorts)->output;
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    void swap(Model& a, Model& b)
    {
        using std::swap;
        swap(a._data, b._data);
    }

    //
    // NodeIterator implementations
    //

    // Base class
    NodeIterator::NodeIterator(const Model* model) :
        _model(model)
    {
    }

    void NodeIterator::SetNodeVisited(const Node* node)
    {
        _visitedNodes.insert(node);
    }

    void NodeIterator::SetSubmodelInputs(const std::vector<const InputPortBase*>& inputs)
    {
        for (const auto& input : inputs)
        {
            _submodelInputs.insert(input);
            auto parents = input->GetParentNodes();
            for (auto parent : parents)
            {
                AddSubmodelInputParents(parent);
            }
        }
    }

    void NodeIterator::AddSubmodelInputParents(const Node* node)
    {
        _submodelInputParents.insert(node);
        for (auto parent : node->GetParentNodes())
        {
            AddSubmodelInputParents(parent);
        }
    }

    void NodeIterator::AddRemainingValidOutputs()
    {
        if (ShouldAddAllValidOutputs())
        {
            // Add everything except inputs on submodelInputs list (and their inputs)
            for (auto node : _model->GetNodeMap())
            {
                const Node* nodePtr = node.second.get();
                if (ShouldAddNodeToValidOutputs(nodePtr))
                {
                    _nodesToVisit.push_back(nodePtr);
                }
            }
        }
    }

    bool NodeIterator::ShouldAddAllValidOutputs() const
    {
        return _nodesToVisit.empty(); // "empty" means "visit full model"
    }

    bool NodeIterator::ShouldAddNodeToValidOutputs(const Node* node) const
    {
        return _submodelInputParents.find(node) == _submodelInputParents.end();
    }

    bool NodeIterator::ShouldVisitInput(const InputPortBase* input) const
    {
        return _submodelInputs.find(input) == _submodelInputs.end();
    }

    void NodeIterator::SetOutputNodesToVisit(const std::vector<const Node*>& outputs)
    {
        _nodesToVisit = outputs;
    }

    void NodeIterator::SetOutputPortsToVisit(const std::vector<const OutputPortBase*>& outputs)
    {
        for (const auto& output : outputs)
        {
            _nodesToVisit.push_back(output->GetNode());
        }
    }

    // ForwardNodeIterator
    ForwardNodeIterator::ForwardNodeIterator(const Model* model, const std::vector<const OutputPortBase*>& outputs) :
        NodeIterator(model)
    {
        if (_model->Size() == 0)
        {
            return;
        }

        SetOutputPortsToVisit(outputs);
        AddRemainingValidOutputs();
        Next();
    }

    ForwardNodeIterator::ForwardNodeIterator(const Model* model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) :
        NodeIterator(model)
    {
        if (_model->Size() == 0)
        {
            return;
        }

        SetSubmodelInputs(inputs);
        SetOutputPortsToVisit(outputs);
        AddRemainingValidOutputs();
        Next();
    }

    void ForwardNodeIterator::Next()
    {
        _currentNode = nullptr;
        while (!_nodesToVisit.empty())
        {
            const Node* node = _nodesToVisit.back();

            // check if we've already visited this node
            if (_visitedNodes.find(node) != _visitedNodes.end())
            {
                _nodesToVisit.pop_back();
                continue;
            }

            // we can visit this node only if all its inputs have been visited already
            bool canVisit = true;
            const auto& inputPorts = node->GetInputPorts();
            for (auto inputPort : inputPorts)
            {
                if (ShouldVisitInput(inputPort))
                {
                    for (const auto& parentNode : inputPort->GetParentNodes())
                    {
                        canVisit = canVisit && _visitedNodes.find(parentNode) != _visitedNodes.end();
                    }
                }
            }

            if (canVisit)
            {
                _nodesToVisit.pop_back();
                SetNodeVisited(node);
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
                        _nodesToVisit.push_back(parentNode);
                    }
                }
            }
        }
    }

    // ReverseNodeIterator
    ReverseNodeIterator::ReverseNodeIterator(const Model* model, const std::vector<const OutputPortBase*>& outputs) :
        NodeIterator(model)
    {
        if (_model->Size() == 0)
        {
            return;
        }

        SetOutputPortsToVisit(outputs);
        AddRemainingValidOutputs();
        Next();
    }

    ReverseNodeIterator::ReverseNodeIterator(const Model* model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) :
        NodeIterator(model)
    {
        if (_model->Size() == 0)
        {
            return;
        }

        SetSubmodelInputs(inputs);
        SetOutputPortsToVisit(outputs);
        AddRemainingValidOutputs();
        Next();
    }

    void ReverseNodeIterator::Next()
    {
        _currentNode = nullptr;
        while (!_nodesToVisit.empty())
        {
            const Node* node = _nodesToVisit.back();

            // check if we've already visited this node
            if (_visitedNodes.find(node) != _visitedNodes.end())
            {
                _nodesToVisit.pop_back();
                continue;
            }

            // we can visit this node only if all its outputs have been visited already
            bool canVisit = true;
            const auto children = node->GetDependentNodes();
            for (const auto& childNode : children)
            {
                canVisit = canVisit && _visitedNodes.find(childNode) != _visitedNodes.end();
            }

            if (canVisit)
            {
                _nodesToVisit.pop_back();
                _visitedNodes.insert(node);
                _currentNode = node;
                break;
            }
            else // visit node's outputs
            {
                const auto children = node->GetDependentNodes();
                for (const auto& childNode : children)
                {
                    _nodesToVisit.push_back(childNode);
                }
            }
        }
    }

    //
    // ModelSerializationContext
    //
    ModelSerializationContext::ModelSerializationContext(utilities::SerializationContext& previousContext, Model* model) :
        utilities::SerializationContext(previousContext, {}),
        _model(model)
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
} // namespace model
} // namespace ell
