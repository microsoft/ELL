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

#include <unordered_set>
#include <vector>
#include <memory>
#include <unordered_map>
#include <exception>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> Model class. Represents a graph of computation </summary>
    class Model
    {
    public:
        /// <summary> Factory method used to create nodes and add them to the graph. </summary>
        template <typename NodeType, typename... Args>
        std::shared_ptr<NodeType> AddNode(Args&&... args);

        /// <summary> Looks up a node by id </summary>
        ///
        /// <param name="id"> The id of the node </param>
        /// <returns> a weak_ptr to the node </param>
        std::weak_ptr<Node> GetNode(Node::NodeId id);

        /// <summary> Returns part of the output computed by a node </summary>
        ///
        /// <param name="outputPort"> The output port to get the computed value form </param>
        template <typename ValueType>
        std::vector<ValueType> GetNodeOutput(const OutputPort<ValueType>& outputPort) const;

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
        void Visit(Visitor&& visitor, const std::shared_ptr<Node>& outputNode) const;

        /// <summary>
        /// Visits the nodes in the graph necessary to compute the outputs of the given nodes. Visits the nodes
        /// in dependency order. No nodes will be visited until all its inputs have first been visited.
        /// </summary>
        ///
        /// <param name="visitor"> The visitor functor to use </param>
        /// <param name="outputNode"> The output node to use for deciding which nodes to visit </param>
        template <typename Visitor>
        void Visit(Visitor&& visitor, const std::vector<std::shared_ptr<Node>>& outputNodes) const;

        // TODO: iterators, including begin/end for iterating over entire graph

        Model Copy() const;
        Model Refine() const;

    private:
        // The id->node map acts both as the main container that holds the shared pointers to nodes, and as the index
        // to look nodes up by id.
        std::unordered_map<Node::NodeId, std::shared_ptr<Node>> _nodeMap;

        // This is the single implementation of Visit, invoked by the publicly-visible ones
        template <typename Visitor>
        void Visit(Visitor&& visitor, const std::vector<const Node*>& outputNodePtrs) const;
    };


    // TODO: maybe ModelTransformer contains the new model, instead of having it in the Refine/Transform method?
    // Stuff for refinement / copying
    class ModelTransformer
    {
    public:
        void MapPort(const Port* oldPort, Port* newPort)
        {
            _portMap[oldPort] = newPort;
        }

        template <typename ValueType>
        OutputPortElementList<ValueType> CopyInputPort(const InputPort<ValueType>& input)
        {
            const auto& ranges = input.GetInputRanges();
            std::vector<OutputPortElementList<ValueType>> newRanges;
            for (const auto& range : ranges)
            {
                auto oldPort = range.ReferencedPort();
                auto newPort = _portMap[oldPort];
                auto outputPort = dynamic_cast<const OutputPort<ValueType>*>(newPort);
                // TODO: ensure dynamic cast worked
                auto start = range.GetStartIndex();
                auto size = range.Size();
                newRanges.emplace_back(*outputPort, start, size);
            }
            return OutputPortElementList<ValueType>(newRanges);
        }


    private:
        std::unordered_map<const Port*, Port*> _portMap;
    };
}

#include "../tcc/ModelGraph.tcc"


