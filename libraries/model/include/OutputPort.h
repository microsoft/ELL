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
        /// <summary> Notify this port that it is being referenced </summary>
        void ReferencePort() const { _isReferenced = true; }

        /// <summary> Indicate if this port is referenced. </summary>
        ///
        /// <returns> Returns true if the port is referenced by another node. </returns>
        bool IsReferenced() { return _isReferenced; }

    protected:
        OutputPortBase(const class Node* node, std::string name, PortType type, size_t size);

        mutable bool _isReferenced;
    };

    /// <summary> Represents an output from a node </summary>
    template <typename ValueType>
    class OutputPort : public OutputPortBase
    {
    public:
        OutputPort(const OutputPort&) = delete;

        /// <summary> Constructor </summary>
        ///
        /// <param name="node"> The node this output port is part of </param>
        /// <param name="name"> The name of this port </param>
        /// <param name="size"> The dimensionality of this port </param>
        OutputPort(const class Node* node, std::string name, size_t size);

        /// <summary> Returns the cached output from this port </summary>
        ///
        /// <returns> The cached output from this port </returns>
        const std::vector<ValueType>& GetOutput() const { return _cachedOutput; }

        /// <summary> Returns one element of the cached output from this port </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The cached output for the element </returns>
        ValueType GetOutput(size_t index) const;

        /// <summary> Sets the cached output from this port </summary>
        ///
        /// <param name=values> The values this port should output </param>
        void SetOutput(std::vector<ValueType> values) const;

		/// <summary> Sets the cached output from this port </summary>
		void SetOutput(ValueType value) const;

    private:
        mutable std::vector<ValueType> _cachedOutput;
    };
}

#include "../tcc/OutputPort.tcc"