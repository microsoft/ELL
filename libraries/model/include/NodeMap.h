////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NodeMap.h (model)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// model
#include "Node.h"

// stl
#include <unordered_map>

namespace ell
{
namespace model
{
    /// <summary> Used by the compiler to maintain tables for data associated with each node. </summary>
    template <typename T, T Default = T(0)>
    class NodeMap
    {
    public:
        /// <summary> Get the mapped value for this node, or the default </summary>
        T Get(const model::Node& node) const;
        /// <summary> Set the mapped value for this node </summary>
        void Set(const model::Node& node, T value);
        /// <summary> True if the given node is mapped </summary>
        bool Contains(const model::Node& node) const;
        /// <summary> Remove the mapped value for this node </summary>
        void Remove(const model::Node& node);
        /// <summary> Clear the map </summary>
        void Clear();

    private:
        std::unordered_map<model::Node::NodeId, T> _map;
    };
}
}

#include "../tcc/NodeMap.tcc"
