////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "OutputPort.h"
#include "Node.h"

#include <vector>
#include <cassert>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> Represents a contiguous set of values from an output port </summary>
    class OutputRange
    {
    public:
        /// <summary> Creates an OutputRange representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputRange(const Port& port);

        /// <summary> Creates an OutputRange representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputRange(const Port& port, size_t index);

        /// <summary> Creates an OutputRange representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputRange(const Port& port, size_t startIndex, size_t numValues);

        /// <summary> Returns the type of the values referenced </summary>
        ///
        /// <returns> The type of the values referenced </returns>
        Port::PortType Type() const { return _referencedPort->Type(); }

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const;

        /// <summary> The port this range refers to </summary>
        ///
        /// <returns> The port this range refers to </returns>
        const Port* ReferencedPort() const { return _referencedPort; }

    private:
        friend class InputPort;

        const Port* _referencedPort;
        size_t _startIndex;
        size_t _numValues;
        bool _isFixedSize;
    };

    /// <summary> Represents a set of values from one or more output ports </summary>
    class OutputRef
    {
    public:
        /// <summary> Creates an OutputRef representing all the values from a given port </summary>
        OutputRef(const Port& port);

        /// <summary> Creates an OutputRef representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputRef(const Port& port, size_t startIndex);

        /// <summary> Creates an OutputRef representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputRef(const Port& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an OutputRef from an OutputRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        OutputRef(const OutputRange& range);

        /// <summary> Creates an OutputRef from a set of OutputRange </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        OutputRef(const std::vector<OutputRange>& ranges);

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<OutputRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<OutputRange>::const_iterator end() const { return _ranges.cend(); }

    protected:
        OutputRef(){};
        void ComputeSize();

        std::vector<OutputRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class TypedOutputRef : public OutputRef
    {
    public:
        /// <summary> Creates a TypedOutputRef representing all the values from a given port </summary>
        TypedOutputRef(const OutputPort<ValueType>& port);

        /// <summary> Creates a TypedOutputRef representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        TypedOutputRef(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a TypedOutputRef representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        TypedOutputRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a TypedOutputRef by concatenating a set of them together </summary> 
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        TypedOutputRef(const std::initializer_list<TypedOutputRef<ValueType>>& groups);

        /// <summary> Creates a TypedOutputRef by concatenating a set of them together </summary> 
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        TypedOutputRef(const std::vector<TypedOutputRef<ValueType>>& groups);
    };

    /// <summary> Class representing an input to a node </summary>
    class InputPort : public Port
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="owningNode"> The node that contains this port </param>
        /// <param name="portIndex"> The index of this port within the owning node <param>
        /// <param name="input"> The input group to fetch input values from </param>
        template <typename ValueType>
        InputPort(const class Node* owningNode, size_t portIndex, const TypedOutputRef<ValueType>& input);

        /// <summary> Returns the OutputRef containing the ranges of referenced locations to get values from </summary>
        ///
        /// <returns> The OutputRef containing the ranges of referenced locations to get values from </returns>
        const OutputRef& GetInputRanges() const { return _inputRanges; }

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _inputRanges.Size(); }

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        ///
        /// <returns> The (already-computed) output value corresponding to this input </returns>
        template <typename ValueType>
        std::vector<ValueType> GetValue() const;

    private:
        OutputRef _inputRanges;
    };
}

#include "../tcc/InputPort.tcc"
