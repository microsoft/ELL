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

    /// <summary> An iterator over the nodes in a Model </summary>
    class NodeIterator : public utilities::IIterator<const Node*>
    {
    public:
        NodeIterator() = default;

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if valid, false if not. </returns>
        bool IsValid() const override { return _currentNode != nullptr; }

        /// <summary> Proceeds to the Next item. </summary>
        void Next() override;

        /// <summary> Returns a const reference to the current item. </summary>
        ///
        /// <returns> A const reference to the current item; </returns>
        const Node* Get() const override { return _currentNode; }

    private:
        friend class Model;
        NodeIterator(const Model* model, const std::vector<const Node*>& outputNodes);

        const Model* _model = nullptr;
        std::unordered_set<const Node*> _visitedNodes;
        std::vector<const Node*> _stack;

        const Node* _currentNode = nullptr;
    };

    /// <summary> Model class. Represents a graph of computation </summary>
    class Model : public utilities::IArchivable
    {
    public:
        /// <summary> Factory method used to create nodes and add them to the model. </summary>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

        /// <summary> Looks up a node by id </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> a weak_ptr to the node </param>
        Node* GetNode(Node::NodeId id);

        /// <summary> Looks up a node by id </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> a weak_ptr to the node </param>
        const Node* GetNode(Node::NodeId id) const;

        /// <summary> Get number of nodes </summary>
        ///
        /// <returns> The number of nodes in the model </summary>
        size_t Size() const { return _idToNodeMap.size(); }

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
        /// Gets an iterator over all the nodes in the model in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        NodeIterator GetNodeIterator() const { return GetNodeIterator(std::vector<const Node*>{}); }

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the output of a given node. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputNode"> The output node to use for deciding which nodes to visit </param>
        NodeIterator GetNodeIterator(const Node* outputNode) const { return GetNodeIterator(std::vector<const Node*>{ outputNode }); }

        /// <summary>
        /// Gets an iterator over the nodes in the model necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputNodes"> The output nodes to use for deciding which nodes to visit </param>
        NodeIterator GetNodeIterator(const std::vector<const Node*>& outputNodes) const;

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
        utilities::PropertyBag& GetMetadata() { return _metadata; }

        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A const reference to the PropertyBag containing the metadata for this object. </returns>
        const utilities::PropertyBag& GetMetadata() const { return _metadata; }
        
    protected:
        // Serialization-related methods
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        friend class NodeIterator;

        // The id->node map acts both as the main container that holds the shared pointers to nodes, and as the index
        // to look nodes up by id.
        // We keep it sorted by id to make visiting all nodes deterministically ordered
        std::map<Node::NodeId, std::shared_ptr<Node>, std::less<Node::NodeId>> _idToNodeMap;
        utilities::PropertyBag _metadata;
    };

    /// <summary> A serialization context used during model deserialization. Wraps an existing `SerializationContext`
    /// and adds access to the model being constructed. </summary>
    class ModelSerializationContext : public utilities::SerializationContext
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="model"> The model being constructed </param>
        ModelSerializationContext(utilities::SerializationContext& previousContext, const Model* model);

        /// <summary> Sets the model this map is deserializing
        ///
        /// <param name="model"> The model this map wraps </param>
        void SetModel(const Model* model);

        /// <summary> Returns the Model currently being deserialized. </summary>
        ///
        /// <returns> The Model currently being deserialized. </returns>
        const Model* GetModel() { return _model; }

        /// <summary> Returns a pointer to an already-deserialized node, given its serialized ID </summary>
        ///
        /// <returns> A pointer to an already-deserialized node. </returns>
        Node* GetNodeFromSerializedId(const Node::NodeId& id);

        /// <summary> Associate a newly-deserialized node with its serialized ID </summary>
        ///
        /// <param name="id"> The serialized ID of the node. </param>
        /// <param name="node"> A pointer to the serialized node. </param>
        void MapNode(const Node::NodeId& id, Node* node);

    private:
        const Model* _model;
        std::unordered_map<Node::NodeId, Node*> _oldToNewNodeMap;
    };
}
}

#include "../tcc/Model.tcc"
