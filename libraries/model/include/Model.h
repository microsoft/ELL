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

// utilities
#include "IArchivable.h"
#include "IIterator.h"
#include "PropertyBag.h"

// stl
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
        NodeIterator() = default;
        NodeIterator(const Model* model);

        const Model* _model = nullptr;
        std::unordered_set<const Node*> _visitedNodes;
        std::vector<const Node*> _stack;

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
        ForwardNodeIterator(const Model* model, const std::vector<const Node*>& outputNodes);
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
    };

    /// <summary> Model class. Represents a graph of computation </summary>
    class Model : public utilities::IArchivable
    {
    public:
        Model();
        Model(Model&& other) = default;
        Model& operator=(Model&& other) = default;

        /// <summary> Explicit method to create a shallow copy </summary>
        Model ShallowCopy() const;

        /// <summary> Factory method used to create nodes and add them to the model. </summary>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

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
        /// <param name="outputPort"> The output port to get the computed value form </param>
        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const OutputPort<ValueType>& outputPort) const;

        /// <summary> Returns part of the output computed by the model </summary>
        ///
        /// <param name="elements"> The output port elements to get the computed value form </param>
        template <typename ValueType>
        std::vector<ValueType> ComputeOutput(const PortElements<ValueType>& elements) const;

        /// <summary> Returns part of the output computed by the model </summary>
        ///
        /// <param name="elements"> The output port elements to get the computed value form </param>
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
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        /// <param name="outputNode"> The output node to use for deciding which nodes to visit </param>
        template <typename Visitor>
        void VisitSubset(const Node* outputNode, Visitor&& visitor) const;

        /// <summary>
        /// Visits the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        /// <param name="outputNodes"> The output nodes to use for deciding which nodes to visit </param>
        template <typename Visitor>
        void VisitSubset(const std::vector<const Node*>& outputNodes, Visitor&& visitor) const;

        /// <summary>
        /// Visits all the nodes in the model in reverse dependency order. No nodes will be visited until all
        /// its outputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use. The type signature should be of the form `void visitor(const Node&)`. </param>
        template <typename Visitor>
        void ReverseVisit(Visitor&& visitor) const;

        /// <summary>
        /// Gets an iterator over all the nodes in the model in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        ForwardNodeIterator GetNodeIterator() const { return GetNodeIterator(std::vector<const Node*>{}); }

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the output of a given node. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputNode"> The output node to use for deciding which nodes to visit </param>
        ForwardNodeIterator GetNodeIterator(const Node* outputNode) const { return GetNodeIterator(std::vector<const Node*>{ outputNode }); }

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputNodes"> The output nodes to use for deciding which nodes to visit </param>
        ForwardNodeIterator GetNodeIterator(const std::vector<const Node*>& outputNodes) const;

        /// <summary>
        /// Gets an iterator over all the nodes in the model in reverse dependency order. No nodes will be visited until all
        /// its outputs have first been visited.
        /// </summary>
        ReverseNodeIterator GetReverseNodeIterator() const;

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
        void PrintSubset(std::ostream& os, const Node* outputNode) const;

        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A reference to the PropertyBag containing the metadata for this object. </returns>
        utilities::PropertyBag& GetMetadata() { return _data->metadata; }

        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A const reference to the PropertyBag containing the metadata for this object. </returns>
        const utilities::PropertyBag& GetMetadata() const { return _data->metadata; }
            
    protected:
        // Serialization-related methods
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

        struct ModelData
        {
            // The id->node map acts both as the main container that holds the shared pointers to nodes, and as the index
            // to look nodes up by id.
            // We keep it sorted by id to make visiting all nodes deterministically ordered
            std::map<Node::NodeId, std::shared_ptr<Node>, std::less<Node::NodeId>> idToNodeMap;
            utilities::PropertyBag metadata;
        };

        Model(const std::shared_ptr<Model::ModelData>& data);
        Model(const Model& other) = delete;

        template <typename ValueType>
        PortElements<ValueType> AddRoutingNodes(const PortElements<ValueType>& elements);
        PortElementsBase AddRoutingNodes(const PortElementsBase& elements);
        const OutputPortBase* AddPortRange(const PortRange& inputRange);
        const OutputPortBase* AddConcat(const std::vector<const OutputPortBase*>& outputPorts);
        Node* AddExistingNode(std::unique_ptr<Node> node);
        void EnsureNodeHasUniqueId(Node& node);
        Node::NodeId GetUniqueId(const Node::NodeId& desiredId);
        static Node::NodeId GetNextId(Node::NodeId id);

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
}
}

#include "../tcc/Model.tcc"
