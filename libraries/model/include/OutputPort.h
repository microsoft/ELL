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

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Base class for output ports </summary>
    class OutputPortBase : public Port
    {
    public:
        void ReferencePort() const { _isReferenced = true; }
        bool IsReferenced() { return _isReferenced; }

    protected:
        OutputPortBase(const class Node* node, PortType type, size_t size) : Port(node, type, size), _isReferenced(false) {}

        mutable bool _isReferenced;
    };

    /// <summary> Represents an output from a node </summary>
    template <typename ValueType>
    class OutputPort : public OutputPortBase
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node this output port is part of </param>
        /// <param name="size"> The dimensionality of this port </param>
        OutputPort(const class Node* node, size_t size) : OutputPortBase(node, OutputPortBase::GetTypeCode<ValueType>(), size) {}

        /// <summary> Returns the cached output from this port </summary>
        ///
        /// <returns> The cached output from this port </returns>
        const std::vector<ValueType>& GetOutput() const { return _cachedOutput; }

        /// <summary> Returns one element of the cached output from this port </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The cached output for the element </returns>
        ValueType GetOutput(size_t index) const { return _cachedOutput[index]; }

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        void SetOutput(std::vector<ValueType> values) const { _cachedOutput = values; }

    private:
        mutable std::vector<ValueType> _cachedOutput;
    };
}
