////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

// utilities
#include "IIterator.h"
#include "ISerializable.h"
#include "ObjectDescription.h"

#include <unordered_set>
#include <vector>
#include <memory>
#include <unordered_map>
#include <exception>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> An iterator over the nodes in a Model graph </summary>
    class NodeIterator : public utilities::IIterator<const Node*>
    {
    public:
        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if valid, false if not. </returns>
        virtual bool IsValid() const override { return _currentNode != nullptr; }

        /// <summary> Proceeds to the Next item. </summary>
        virtual void Next() override;

        /// <summary> Returns a const reference to the current item. </summary>
        ///
        /// <returns> A const reference to the current item; </returns>
        virtual const Node* Get() const override { return _currentNode; }

    private:
        friend class Model;
        NodeIterator(const Model* model, const std::vector<const Node*>& outputNodes);

        const Model* _model = nullptr;
        std::unordered_set<const Node*> _visitedNodes;
        std::vector<const Node*> _stack;

        bool _visitFullGraph = false;
        const Node* _currentNode = nullptr;
    };

    /// <summary> Model class. Represents a graph of computation </summary>
    class Model : public utilities::ISerializable
    {
    public:
        /// <summary> Factory method used to create nodes and add them to the graph. </summary>
        template <typename NodeType, typename... Args>
        NodeType* AddNode(Args&&... args);

        /// <summary> Looks up a node by id </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> a weak_ptr to the node </param>
        Node* GetNode(Node::NodeId id);

        /// <summary> Get number of nodes </summary>
        ///
        /// <returns> The number of nodes in the model </summary>
        size_t Size() const { return _idToNodeMap.size(); }

        /// <summary> Retrieves a set of nodes by type </summary>
        ///
        /// <typeparam name="NodeType"> The type of the node </typeparam>
        /// <returns> A vector of nodes of the requested type </returns>
        template <typename NodeType>
        std::vector<const NodeType*> GetNodesByType();

        /// <summary> Returns part of the output computed by a node </summary>
        ///
        /// <param name="outputPort"> The output port to get the computed value form </param>
        template <typename ValueType>
        std::vector<ValueType> ComputeNodeOutput(const OutputPort<ValueType>& outputPort) const;

        /// <summary>
        /// Visits all the nodes in the graph in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use </param>
        template <typename Visitor>
        void Visit(Visitor&& visitor) const;

        /// <summary>
        /// Visits the nodes in the graph necessary to compute the output of a given node. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use </param>
        /// <param name="outputNode"> The output node to use for deciding which nodes to visit </param>
        template <typename Visitor>
        void Visit(Visitor&& visitor, const Node* outputNode) const;

        /// <summary>
        /// Visits the nodes in the graph necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use </param>
        /// <param name="outputNodes"> The output nodes to use for deciding which nodes to visit </param>
        template <typename Visitor>
        void Visit(Visitor&& visitor, const std::vector<const Node*>& outputNodes) const;

        /// <summary>
        /// Gets an iterator over all the nodes in the graph in dependency order. No nodes will be visited until all
        /// its inputs have first been visited.
        /// </summary>
        NodeIterator GetNodeIterator() const { return GetNodeIterator(std::vector<const Node*>{}); }

        /// <summary>
        /// Gets an iterator over the nodes in the graph necessary to compute the output of a given node. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputNode"> The output node to use for deciding which nodes to visit </param>
        NodeIterator GetNodeIterator(const Node* outputNode) const { return GetNodeIterator(std::vector<const Node*>{ outputNode }); }

        /// <summary>
        /// Gets an iterator over the nodes in the graph necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="outputNodes"> The output nodes to use for deciding which nodes to visit </param>
        NodeIterator GetNodeIterator(const std::vector<const Node*>& outputNodes) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ModelGraph"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        /// <summary> ISerializable interface </summary>
        // virtual utilities::ObjectDescription GetDescription() const override;
        virtual void Serialize(utilities::Serializer& serializer) const override;

    private:
        friend class NodeIterator;
        // The id->node map acts both as the main container that holds the shared pointers to nodes, and as the index
        // to look nodes up by id.
        std::unordered_map<Node::NodeId, std::shared_ptr<Node>> _idToNodeMap;
    };
}

#include "../tcc/ModelGraph.tcc"
