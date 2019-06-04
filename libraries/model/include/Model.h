////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "OutputPort.h"
#include "PortElements.h"

#include <utilities/include/IArchivable.h>
#include <utilities/include/IIterator.h>
#include <utilities/include/PropertyBag.h>

#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    class Model;
    namespace detail
    {
        class ModelNodeRouter;
    }

    /// <summary> Abstract base class for iterators over the nodes in a Model </summary>
    class NodeIterator : public utilities::IIterator<const Node*>
    {
    public:
        NodeIterator() = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid node. </summary>
        ///
        /// <returns> true if valid, false if not. </returns>
        bool IsValid() const override { return _currentNode != nullptr; }

        /// <summary> Returns a const reference to the current node. </summary>
        ///
        /// <returns> A const reference to the current node. </returns>
        const Node* Get() const override { return _currentNode; }

    protected:
        friend class Model;
        NodeIterator(const Model* model);
        void SetNodeVisited(const Node* node);
        void SetSubmodelInputs(const std::vector<const InputPortBase*>& inputs);
        void AddSubmodelInputParents(const Node* node);
        void AddRemainingValidOutputs();
        bool ShouldAddAllValidOutputs() const;
        bool ShouldAddNodeToValidOutputs(const Node* node) const;
        bool ShouldVisitInput(const InputPortBase* input) const;
        void SetOutputNodesToVisit(const std::vector<const Node*>& outputs);
        void SetOutputPortsToVisit(const std::vector<const OutputPortBase*>& outputs);

        const Model* _model = nullptr;
        std::unordered_set<const Node*> _visitedNodes;
        std::unordered_set<const InputPortBase*> _submodelInputs;
        std::unordered_set<const Node*> _submodelInputParents;
        std::vector<const Node*> _nodesToVisit;

        const Node* _currentNode = nullptr;
    };

    /// <summary>
    /// An iterator over the nodes in the model that visits the nodes in dependency
    /// order --- no nodes will be visited until all its inputs have first been visited.
    /// Other than fulfilling this constraint, no guarantees are made on the order the
    /// nodes are visited.
    /// </summary>
    class ForwardNodeIterator : public NodeIterator
    {
    public:
        ForwardNodeIterator() = default;

        /// <summary> Proceeds to the next node. </summary>
        void Next() override;

    private:
        friend class Model;
        ForwardNodeIterator(const Model* model, const std::vector<const OutputPortBase*>& outputs);
        ForwardNodeIterator(const Model* model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs);
    };

    /// <summary>
    /// An iterator over the nodes in the model that visits the nodes in reverse dependency
    /// order --- no nodes will be visited until all its outputs have first been visited.
    /// Other than fulfilling this constraint, no guarantees are made on the order the
    /// nodes are visited.
    /// </summary>
    class ReverseNodeIterator : public NodeIterator
    {
    public:
        ReverseNodeIterator() = default;

        /// <summary> Proceeds to the next node. </summary>
        void Next() override;

    private:
        friend class Model;
        ReverseNodeIterator(const Model* model);
        ReverseNodeIterator(const Model* model, const std::vector<const OutputPortBase*>& outputs);
        ReverseNodeIterator(const Model* model, const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs);
    };

    /// <summary> Model class. Represents a computation graph, where each node represents a function,
    /// and function composition is done by connecting node inputs to node outputs. </summary>
    class Model : public utilities::IArchivable
    {
    public:
        Model();
        Model(Model&& other) = default;
        Model& operator=(Model&& other) = default;

        /// <summary> Explicit method to create a shallow copy of the model. </summary>
        ///
        /// <returns>
        /// A shallow copy of the model, containing the same nodes as the original. Any
        /// changes to the original model will be reflected in the copy, and vice-versa.
        /// </returns>
        Model ShallowCopy() const;

        /// <summary> Explicit method to create a deep copy of the model. </summary>
        ///
        /// <returns>
        /// A deep copy of the model. This is an entirely different model, constructed by
        /// cloning each of the nodes in the original model. Any changes made in the original
        /// model won't affect the copy (and vice-versa).
        /// </returns>
        Model DeepCopy() const;

        /// <summary> Factory method used to create nodes and add them to the model. </summary>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

        /// <summary> Adds nodes to the model to represent "complex" outputs: the concatenation of arbitrary subsets of output ports </summary>
        ///
        /// <param name="elements"> The output port elements to simplify to a new output port. </param>
        ///
        /// <returns> An output port containing the values specified by the `elements` argument. </returns>
        template <typename ValueType>
        const OutputPort<ValueType>& SimplifyOutputs(const PortElements<ValueType>& elements);

        /// <summary> Adds nodes to the model to represent "complex" outputs: the concatenation of arbitrary subsets of output ports </summary>
        ///
        /// <param name="elements"> The output port elements to simplify to a new output port. </param>
        ///
        /// <returns> An output port containing the values specified by the `elements` argument. </returns>
        const OutputPortBase& SimplifyOutputs(const PortElementsBase& elements);

        /// <summary> Checks if a node with a given ID is present </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> `true` if a node with the given ID exists </param>
        bool NodeIdExists(Node::NodeId id) const;

        /// <summary> Looks up a node by id </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> a pointer to the node </param>
        Node* GetNode(Node::NodeId id);

        /// <summary> Looks up a node by id </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> a pointer to the node </param>
        const Node* GetNode(Node::NodeId id) const;

        /// <summary> Get number of nodes </summary>
        ///
        /// <returns> The number of nodes in the model </summary>
        size_t Size() const { return _data->idToNodeMap.size(); }

        /// <summary> Retrieves a set of nodes by type </summary>
        ///
        /// <typeparam name="NodeType"> The type of the node </typeparam>
        /// <returns> A vector of nodes of the requested type </returns>
        template <typename NodeType>
        std::vector<const NodeType*> GetNodesByType() const;

        /// <summary> Retrieves a set of nodes by type </summary>
        ///
        /// <typeparam name="NodeType"> The type of the node </typeparam>
        /// <returns> A vector of nodes of the requested type </returns>
        template <typename NodeType>
        std::vector<NodeType*> GetNodesByType();

        /// <summary> Returns part of the output computed by the model </summary>
        ///
        /// <param name="outputPort"> The output port to get the computed value from </param>
        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const OutputPort<ValueType>& outputPort) const;

        /// <summary> Returns part of the output computed by the model </summary>
        ///
        /// <param name="elements"> The output port elements to get the computed value from </param>
        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const PortElements<ValueType>& elements) const;

        /// <summary> Returns part of the output computed by the model </summary>
        ///
        /// <param name="elements"> The output port elements to get the computed value from </param>
        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const PortElementsBase& elements) const;

        /// <summary> Reset the state of the model </summary>
        void Reset();

        /// <summary>
        /// Visits all the nodes in the model in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        template <typename Visitor>
        void Visit(Visitor&& visitor) const;

        /// <summary>
        /// Visits the nodes in the model necessary to compute the output of a given node. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="output"> The output to use for deciding which nodes to visit </param>
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        template <typename Visitor>
        void VisitSubmodel(const OutputPortBase* output, Visitor&& visitor) const;

        /// <summary>
        /// Visits the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputs"> The outputs to use for deciding which nodes to visit </param>
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        template <typename Visitor>
        void VisitSubmodel(const std::vector<const OutputPortBase*>& outputs, Visitor&& visitor) const;

        /// <summary>
        /// Visits the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="inputs"> The inputs to use for deciding which nodes to visit </param>
        /// <param name="outputs"> The outputs to use for deciding which nodes to visit </param>
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        template <typename Visitor>
        void VisitSubmodel(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs, Visitor&& visitor) const;

        /// <summary>
        /// Gets an iterator over all the nodes in the model in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        ForwardNodeIterator GetNodeIterator() const;

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the given output. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="output"> The output to use for deciding which nodes to visit </param>
        ForwardNodeIterator GetNodeIterator(const OutputPortBase* output) const;

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the given outputs. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputs"> The outputs to use for deciding which nodes to visit </param>
        ForwardNodeIterator GetNodeIterator(const std::vector<const OutputPortBase*>& outputs) const;

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="inputs"> The ports to use for deciding which nodes to visit --- nodes that are strictly inputs to these ports won't be visited.</param>
        /// <param name="outputs"> The outputs to use for deciding which nodes to visit </param>
        ForwardNodeIterator GetNodeIterator(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) const;

        /// <summary>
        /// Gets an iterator over all the nodes in the model in reverse dependency order. No nodes will be visited until all
        /// its outputs have first been visited.
        /// </summary>
        ReverseNodeIterator GetReverseNodeIterator() const;

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the given output. Visits the nodes
        /// in reverse dependency order. No nodes will be visited until all its outputs have first been visited.
        /// </summary>
        ///
        /// <param name="output"> The output to use for deciding which nodes to visit </param>
        ReverseNodeIterator GetReverseNodeIterator(const OutputPortBase* output) const;

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the given outputs. Visits the nodes
        /// in reverse dependency order. No nodes will be visited until all its outputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputs"> The outputs to use for deciding which nodes to visit </param>
        ReverseNodeIterator GetReverseNodeIterator(const std::vector<const OutputPortBase*>& outputs) const;

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in reverse dependency order. No nodes will be visited until all its outputs have first been visited.
        /// </summary>
        ///
        /// <param name="inputs"> The ports to use for deciding which nodes to visit --- nodes that are strictly inputs to these ports won't be visited.</param>
        /// <param name="outputs"> The outputs to use for deciding which nodes to visit </param>
        ReverseNodeIterator GetReverseNodeIterator(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "Model"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Print a human-readable representation of the model. </summary>
        ///
        /// <param name="os"> The stream to write data to. </param>
        void Print(std::ostream& os) const;

        /// <summary> Print a human-readable representation of the portion of the model necessary to compute the given output node. </summary>
        ///
        /// <param name="os"> The stream to write data to. </param>
        /// <param name="outputNode"> The node to be computed. </param>
        void PrintSubset(std::ostream& os, const OutputPortBase* outputNode) const;

        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A reference to the PropertyBag containing the metadata for this object. </returns>
        utilities::PropertyBag& GetMetadata() { return _data->metadata; }

        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A const reference to the PropertyBag containing the metadata for this object. </returns>
        const utilities::PropertyBag& GetMetadata() const { return _data->metadata; }

        /// <summary> Equality operator </summary>
        ///
        /// <param name="other"> Model to compare to </param>
        ///
        /// <returns> `true` if this model is idential to the other model (they are the same object or are shallow copies of one another), otherwise `false` </returns>
        bool operator==(const Model& other) const;

        /// <summary> Inequality operator </summary>
        ///
        /// <param name="other"> Model to compare to </param>
        ///
        /// <returns> `false` if this model is idential to the other model (they are the same object or are shallow copies of one another), otherwise `true` </returns>
        bool operator!=(const Model& other) const;

        /// <summary> Perform one compute operation on all nodes.  This assumes input has already been set </summary>
        void Step();

    protected:
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        friend class NodeIterator;
        friend class ForwardNodeIterator;
        friend class ReverseNodeIterator;
        friend class detail::ModelNodeRouter;
        template <typename ValueType>
        friend class InputPort;
        friend class ModelTransformer;
        friend class Map;
        friend void swap(Model& a, Model& b);

        using IDToNodeMap = std::map<Node::NodeId, std::shared_ptr<Node>, std::less<Node::NodeId>>;
        struct ModelData
        {
            // The id->node map acts both as the main container that holds the shared pointers to nodes, and as the index
            // to look nodes up by id.
            // We keep it sorted by id to make visiting all nodes deterministically ordered
            IDToNodeMap idToNodeMap;
            utilities::PropertyBag metadata;
        };

        Model(const std::shared_ptr<Model::ModelData>& data);
        Model(const Model& other) = delete;

        const OutputPortBase& AddSliceNode(const PortRange& inputRange);
        const OutputPortBase& AddSpliceNode(const std::vector<const OutputPortBase*>& outputPorts);
        Node* AddExistingNode(std::unique_ptr<Node> node);
        void EnsureNodeHasUniqueId(Node& node);
        void Verify() const;
        void VerifyNodes() const;
        void VerifyInputs() const;
        void VerifyInputs(const Node& node) const;
        Node::NodeId GetUniqueId(const Node::NodeId& desiredId);
        static Node::NodeId GetNextId(Node::NodeId id);
        const IDToNodeMap& GetNodeMap() const;

        template <typename Visitor>
        void VisitIteratedNodes(NodeIterator& iter, Visitor&& visitor) const;

        std::shared_ptr<ModelData> _data;
    };

    /// <summary> A serialization context used during model deserialization. Wraps an existing `SerializationContext`
    /// and adds access to the model being constructed. </summary>
    class ModelSerializationContext : public utilities::SerializationContext
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model being constructed </param>
        ModelSerializationContext(utilities::SerializationContext& previousContext, Model* model);

        /// <summary> Sets the model this map is deserializing
        ///
        /// <param name="model"> The model this map wraps </param>
        void SetModel(Model* model);

        /// <summary> Returns the Model currently being deserialized. </summary>
        ///
        /// <returns> The Model currently being deserialized. </returns>
        const Model* GetModel() const { return _model; }

        /// <summary> Returns the Model currently being deserialized. </summary>
        ///
        /// <returns> The Model currently being deserialized. </returns>
        Model* GetModel() { return _model; }

        /// <summary> Returns a pointer to an already-deserialized node, given its serialized ID </summary>
        ///
        /// <returns> A pointer to an already-deserialized node. </returns>
        Node* GetNodeFromSerializedId(const Node::NodeId& id) const;

        /// <summary> Associate a newly-deserialized node with its serialized ID </summary>
        ///
        /// <param name="id"> The serialized ID of the node. </param>
        /// <param name="node"> A pointer to the serialized node. </param>
        void MapNode(const Node::NodeId& id, Node* node);

    private:
        Model* _model;
        std::unordered_map<Node::NodeId, Node*> _oldToNewNodeMap;
    };
} // namespace model
} // namespace ell

#pragma region implementation

namespace ell
{
namespace model
{
    namespace detail
    {
        class ModelNodeRouter
        {
        public:
            template <typename T>
            static T&& ConvertPortElementsArgImpl(Model& model, T&& arg, std::false_type, bool)
            {
                // pass through
                return std::forward<T>(arg);
            }

            template <typename T>
            static auto& ConvertPortElementsArgImpl(Model& model, T&& arg, std::true_type, std::false_type)
            {
                // should not use initializer list
                return model.SimplifyOutputs(std::forward<T>(arg));
            }

            template <typename T>
            static auto ConvertPortElementsArgImpl(Model& model, T&& arg, std::true_type, std::true_type)
            {
                // should use initializer list
                return model.SimplifyOutputs({ std::forward<T>(arg) });
            }

            template <typename T>
            static decltype(auto) ConvertPortElementsArg(Model& model, T&& arg)
            {
                constexpr auto noPassThrough =
                    std::is_base_of<PortRange, std::decay_t<T>>{} ||
                    std::is_base_of<PortElementBase, std::decay_t<T>>{} ||
                    std::is_base_of<PortElementsBase, std::decay_t<T>>{};

                constexpr auto shouldUseInitList = !std::is_base_of<PortElementsBase, std::decay_t<T>>{};

                return ConvertPortElementsArgImpl(
                    model,
                    std::forward<T>(arg),
                    std::integral_constant<bool, noPassThrough>{},
                    std::integral_constant<bool, shouldUseInitList>{});
            }
        };

        void LogNewNode(Node* node);
    } // namespace detail

    //
    // Factory method for creating nodes
    //
    template <typename NodeType, typename... Args>
    NodeType* Model::AddNode(Args&&... args)
    {
        auto node = std::make_unique<NodeType>(detail::ModelNodeRouter::ConvertPortElementsArg(*this, std::forward<Args>(args))...);
        auto result = node.get();

        detail::LogNewNode(result);

        AddExistingNode(std::move(node));
        return result;
    }

    template <typename ValueType>
    const OutputPort<ValueType>& Model::SimplifyOutputs(const PortElements<ValueType>& elements)
    {
        const OutputPortBase& port = SimplifyOutputs(static_cast<const PortElementsBase&>(elements));
        return static_cast<const OutputPort<ValueType>&>(port);
    }

    //
    // Compute output value
    //
    template <typename ValueType>
    std::vector<ValueType> Model::ComputeOutput(const OutputPort<ValueType>& outputPort) const
    {
        auto compute = [](const Node& node) { node.Compute(); };
        VisitSubmodel({ &outputPort }, compute);
        return outputPort.GetOutput();
    }

    template <typename ValueType>
    std::vector<ValueType> Model::ComputeOutput(const PortElements<ValueType>& elements) const
    {
        // get set of ports to make sure we visit
        std::unordered_set<const OutputPortBase*> usedPorts;
        for (const auto& range : elements.GetRanges())
        {
            usedPorts.insert(range.ReferencedPort());
        }

        auto ports = std::vector<const OutputPortBase*>(usedPorts.begin(), usedPorts.end());
        VisitSubmodel(ports, [](const Node& node) {
            node.Compute();
        });

        // Now construct the output
        auto numElements = elements.Size();
        std::vector<ValueType> result(numElements);
        for (size_t index = 0; index < numElements; ++index)
        {
            auto element = elements.GetElement(index);
            auto port = element.ReferencedPort();
            auto portOutput = port->GetOutput()[element.GetIndex()];
            result[index] = portOutput;
        }
        return result;
    }

    template <typename ValueType>
    std::vector<ValueType> Model::ComputeOutput(const PortElementsBase& elements) const
    {
        auto typedElements = PortElements<ValueType>(elements);
        return ComputeOutput(typedElements);
    }

    //
    // Get nodes by type
    //
    template <typename NodeType>
    std::vector<const NodeType*> Model::GetNodesByType() const
    {
        std::vector<const NodeType*> result;
        auto findNodes = [&result](const Node& node) {
            auto nodePtr = dynamic_cast<const NodeType*>(&node);
            if (nodePtr != nullptr)
            {
                result.push_back(nodePtr);
            }
        };
        Visit(findNodes);
        return result;
    }

    template <typename NodeType>
    std::vector<NodeType*> Model::GetNodesByType()
    {
        std::vector<NodeType*> result;
        auto findNodes = [&result](const Node& node) {
            auto nodePtr = dynamic_cast<const NodeType*>(&node);
            if (nodePtr != nullptr)
            {
                result.push_back(const_cast<NodeType*>(nodePtr));
            }
        };
        Visit(findNodes);
        return result;
    }

    //
    // Visitors
    //

    // Visits the entire model
    template <typename Visitor>
    void Model::Visit(Visitor&& visitor) const
    {
        std::vector<const OutputPortBase*> emptyVec;
        VisitSubmodel(emptyVec, visitor);
    }

    // Visits just the parts necessary to compute output node
    template <typename Visitor>
    void Model::VisitSubmodel(const OutputPortBase* output, Visitor&& visitor) const
    {
        auto iter = GetNodeIterator(output);
        VisitIteratedNodes(iter, visitor);
    }

    template <typename Visitor>
    void Model::VisitSubmodel(const std::vector<const OutputPortBase*>& outputs, Visitor&& visitor) const
    {
        auto iter = GetNodeIterator(outputs);
        VisitIteratedNodes(iter, visitor);
    }

    template <typename Visitor>
    void Model::VisitSubmodel(const std::vector<const InputPortBase*>& inputs, const std::vector<const OutputPortBase*>& outputs, Visitor&& visitor) const
    {
        auto iter = GetNodeIterator(inputs, outputs);
        VisitIteratedNodes(iter, visitor);
    }

    // Base implementation for "Visit" methods
    template <typename Visitor>
    void Model::VisitIteratedNodes(NodeIterator& iter, Visitor&& visitor) const
    {
        while (iter.IsValid())
        {
            visitor(*iter.Get());
            iter.Next();
        }
    }
} // namespace model
} // namespace ell

#pragma endregion implementation
