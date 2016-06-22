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
    class InputRange
    {
    public:
        /// <summary> Creates an InputRange representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        InputRange(const Port& port);

        /// <summary> Creates an InputRange representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        InputRange(const Port& port, size_t index);

        /// <summary> Creates an InputRange representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        InputRange(const Port& port, size_t startIndex, size_t numValues);

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
    class InputGroup
    {
    public:
        /// <summary> Creates an InputGroup representing all the values from a given port </summary>
        InputGroup(const Port& port);

        /// <summary> Creates an InputGroup representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        InputGroup(const Port& port, size_t startIndex);

        /// <summary> Creates an InputGroup representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        InputGroup(const Port& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an InputGroup from an InputRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        InputGroup(const InputRange& range);

        /// <summary> Creates an InputGroup from a set of InputRanges </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        InputGroup(const std::vector<InputRange>& ranges);

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<InputRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<InputRange>::const_iterator end() const { return _ranges.cend(); }

    protected:
        InputGroup(){};
        void ComputeSize();

        std::vector<InputRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class TypedInputGroup : public InputGroup
    {
    public:
        /// <summary> Creates a TypedInputGroup representing all the values from a given port </summary>
        TypedInputGroup(const OutputPort<ValueType>& port);

        /// <summary> Creates a TypedInputGroup representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        TypedInputGroup(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a TypedInputGroup representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        TypedInputGroup(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a TypedInputGroup by concatenating a set of them together </summary> 
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        TypedInputGroup(const std::initializer_list<TypedInputGroup<ValueType>>& groups);

        /// <summary> Creates a TypedInputGroup by concatenating a set of them together </summary> 
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        TypedInputGroup(const std::vector<TypedInputGroup<ValueType>>& groups);
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
        InputPort(const class Node* owningNode, size_t portIndex, const TypedInputGroup<ValueType>& input);

        /// <summary> Returns the InputGroup containing the ranges of referenced locations to get values from </summary>
        ///
        /// <returns> The InputGroup containing the ranges of referenced locations to get values from </returns>
        const InputGroup& GetInputRanges() const { return _inputRanges; }

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
        InputGroup _inputRanges;
    };
}

#include "../tcc/InputPort.tcc"
