////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"

#include <vector>
#include <memory>
#include <iostream>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Define a type named OutputPortBase to act as a non-templated base class for all output ports </summary>
    typedef Port OutputPortBase;    

    /// <summary> Represents an output from a node </summary>
    template <typename ValueType>
    class OutputPort : public OutputPortBase
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node this output port is part of </param>
        /// <param name="outputIndex"> The index in the list of the node's outputs for this port </param>
        /// <param name="size"> The dimensionality of this port </param>
        OutputPort(const class Node* node, size_t outputIndex, size_t size) : OutputPortBase(node, outputIndex, OutputPortBase::GetTypeCode<ValueType>(), size) {}

        /// <summary> Returns the cached output from this port </summary>
        std::vector<ValueType> GetOutput() const { return _cachedOutput; }

        /// <summary> Sets the cached output from this port </summary>
        void SetOutput(std::vector<ValueType> values) const { _cachedOutput = values; }

    private:
        mutable std::vector<ValueType> _cachedOutput;
    };
}
