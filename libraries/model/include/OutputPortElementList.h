////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortElementList.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "OutputPort.h"
//#include "Node.h"

#include <vector>
#include <cassert>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Represents a reference to a single element in another node's output </summary>
    class OutputPortElement
    {
    public:
        /// <summary> Creates an OutputPortElement representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortElement(const OutputPortBase& port, size_t index);

        /// <summary> Returns the type of the values referenced </summary>
        ///
        /// <returns> The type of the values referenced </returns>
        Port::PortType GetType() const { return _referencedPort->GetType(); }

        /// <summary> The index of the element this range refers to </summary>
        ///
        /// <returns> The index of the element this range refers to </returns>
        size_t GetIndex() const { return _index; }

        /// <summary> The port this range refers to </summary>
        ///
        /// <returns> The port this range refers to </returns>
        const OutputPortBase* ReferencedPort() const { return _referencedPort; }

    private:
        const OutputPortBase* _referencedPort;
        size_t _index;
    };

    /// <summary> Represents a contiguous set of values from an output port </summary>
    class OutputPortRange
    {
    public:
        /// <summary> Creates an OutputPortRange representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortRange(const OutputPortBase& port);

        /// <summary> Creates an OutputPortRange representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortRange(const OutputPortBase& port, size_t index);

        /// <summary> Creates an OutputPortRange representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortRange(const OutputPortBase& port, size_t startIndex, size_t numValues);

        /// <summary> Returns the type of the values referenced </summary>
        ///
        /// <returns> The type of the values referenced </returns>
        Port::PortType GetType() const { return _referencedPort->GetType(); }

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const;

        /// <summary> The index of the first element this range refers to </summary>
        ///
        /// <returns> The index of the first element this range refers to </returns>
        size_t GetStartIndex() const;

        /// <summary> The port this range refers to </summary>
        ///
        /// <returns> The port this range refers to </returns>
        const OutputPortBase* ReferencedPort() const { return _referencedPort; }

    private:
        const OutputPortBase* _referencedPort;
        size_t _startIndex;
        size_t _numValues;
        bool _isFixedSize;
    };

    /// <summary> Represents a set of values from one or more output ports </summary>
    class OutputPortElementListUntyped
    {
    public:
        /// <summary> Creates an OutputPortElementListUntyped representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortElementListUntyped(const OutputPortBase& port);

        /// <summary> Creates an OutputPortElementListUntyped representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortElementListUntyped(const OutputPortBase& port, size_t startIndex);

        /// <summary> Creates an OutputPortElementListUntyped representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortElementListUntyped(const OutputPortBase& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an OutputPortElementListUntyped from an OutputPortRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        OutputPortElementListUntyped(const OutputPortRange& range);

        /// <summary> Creates an OutputPortElementListUntyped from a set of OutputPortRange </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        OutputPortElementListUntyped(const std::vector<OutputPortRange>& ranges);

        /// <summary> Returns a single-element range for the element at the given index </summary>
        ///
        /// <param name="elementIndex"> The index of the element </param>
        /// <returns> A single-element range for the element </returns>
        // OutputPortRange GetOutputPortElement(size_t elementIndex) const;

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<OutputPortRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<OutputPortRange>::const_iterator end() const { return _ranges.cend(); }

        void AddRange(const OutputPortRange& range);

    protected:
        OutputPortElementListUntyped(){};
        void ComputeSize();

    private:
        std::vector<OutputPortRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class OutputPortElementList : public OutputPortElementListUntyped
    {
    public:
        OutputPortElementList() = default;

        /// <summary> Creates a OutputPortElementList representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortElementList(const OutputPort<ValueType>& port);

        /// <summary> Creates a OutputPortElementList representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a OutputPortElementList representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a OutputPortElementList by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputPortElementList(const std::initializer_list<OutputPortElementList<ValueType>>& groups);

        /// <summary> Creates a OutputPortElementList by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputPortElementList(const std::vector<OutputPortElementList<ValueType>>& groups);
    };

    //
    // Helper functions
    //

    /// <summary> Creates a OutputPortElementList representing all the values from a given port </summary>
    ///
    /// <param name="port"> The port to take values from </param>
    /// <returns> The composite OutputPortElementList </returns>
    template <typename ValueType>
    OutputPortElementList<ValueType> MakeOutputPortElementList(const OutputPort<ValueType>& port);

    /// <summary> Creates a OutputPortElementList representing a single value from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="index"> The index of the value </param>
    /// <returns> The composite OutputPortElementList </returns>
    template <typename ValueType>
    OutputPortElementList<ValueType> MakeOutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex);

    /// <summary> Creates a OutputPortElementList representing a range of values from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="startIndex"> The index of the first value to take </param>
    /// <param name="numValues"> The number of values to take </param>
    /// <returns> The composite OutputPortElementList </returns>
    template <typename ValueType>
    OutputPortElementList<ValueType> MakeOutputPortElementList(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

    /// <summary> Creates a OutputPortElementList by concatenating together one or more OutputPortElementLists
    ///
    /// <param name="ref"> The OutputPortElementLists to concatenate together </param>
    /// <returns> The composite OutputPortElementList </returns>
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs);
}

#include "../tcc/OutputPortElementList.tcc"
