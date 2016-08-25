////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IsNodeCompilable.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"

// stl
#include <string>
#include <unordered_set>

namespace common
{
    /// <summary> Functor for the default implementation of the IsNodeCompilable function. </summary>
    class IsNodeCompilable
    {
    public:
        /// <summary> Default constructor. </summary>
        IsNodeCompilable();

        /// <summary> Determines if a node is compilable or not </summary>
        ///
        /// <param name="node"> A node. </param>
        ///
        /// <returns> True if the node is compilable. </returns>
        bool operator()(const model::Node& node);

    protected:
        std::unordered_set<std::string> _nodeNames;
    
    private:
        template <typename NodeType>
        void AddNodeType();
    };
}

#include "../tcc/IsNodeCompilable.tcc"