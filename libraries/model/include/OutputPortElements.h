////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortElements.h (model)
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

        /// <summary> Indicates if this range consists of the entire port's output </summary>
        ///
        /// <returns> true if this range spans the port's entire range </returns>
        bool IsFullPortRange() const;

    private:
        const OutputPortBase* _referencedPort;
        size_t _startIndex;
        size_t _numValues;
        bool _isFixedSize;
    };

    /// <summary> Represents a set of values from one or more output ports </summary>
    class OutputPortElementsUntyped
    {
    public:
        /// <summary> Creates an OutputPortElementsUntyped representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortElementsUntyped(const OutputPortBase& port);

        /// <summary> Creates an OutputPortElementsUntyped representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortElementsUntyped(const OutputPortBase& port, size_t startIndex);

        /// <summary> Creates an OutputPortElementsUntyped representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortElementsUntyped(const OutputPortBase& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an OutputPortElementsUntyped from an OutputPortRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        OutputPortElementsUntyped(const OutputPortRange& range);

        /// <summary> Creates an OutputPortElementsUntyped from a set of OutputPortRange </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        OutputPortElementsUntyped(const std::vector<OutputPortRange>& ranges);

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> The number of ranges in this list </summary>
        ///
        /// <returns> The number of ranges in this list </returns>
        size_t NumRanges() const { return _ranges.size(); }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<OutputPortRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<OutputPortRange>::const_iterator end() const { return _ranges.cend(); }

        /// <summary> Adds a range of values to this list </summary>
        ///
        /// <param name="range"> The range of values to add to this list </param>
        void AddRange(const OutputPortRange& range);

    protected:
        OutputPortElementsUntyped(){};
        void ComputeSize();

    private:
        std::vector<OutputPortRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class OutputPortElements : public OutputPortElementsUntyped
    {
    public:
        OutputPortElements() = default;

        /// <summary> Creates a OutputPortElements representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortElements(const OutputPort<ValueType>& port);

        /// <summary> Creates a OutputPortElements representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortElements(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a OutputPortElements representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a OutputPortElements by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputPortElements(const std::initializer_list<OutputPortElements<ValueType>>& groups);

        /// <summary> Creates a OutputPortElements by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputPortElements(const std::vector<OutputPortElements<ValueType>>& groups);
    };

    //
    // Helper functions
    //

    /// <summary> Creates a OutputPortElements representing all the values from a given port </summary>
    ///
    /// <param name="port"> The port to take values from </param>
    /// <returns> The composite OutputPortElements </returns>
    template <typename ValueType>
    OutputPortElements<ValueType> MakeOutputPortElements(const OutputPort<ValueType>& port);

    /// <summary> Creates a OutputPortElements representing a single value from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="index"> The index of the value </param>
    /// <returns> The composite OutputPortElements </returns>
    template <typename ValueType>
    OutputPortElements<ValueType> MakeOutputPortElements(const OutputPort<ValueType>& port, size_t startIndex);

    /// <summary> Creates a OutputPortElements representing a range of values from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="startIndex"> The index of the first value to take </param>
    /// <param name="numValues"> The number of values to take </param>
    /// <returns> The composite OutputPortElements </returns>
    template <typename ValueType>
    OutputPortElements<ValueType> MakeOutputPortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

    /// <summary> Creates a OutputPortElements by concatenating together one or more OutputPortElementss
    ///
    /// <param name="ref"> The OutputPortElementss to concatenate together </param>
    /// <returns> The composite OutputPortElements </returns>
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs);
}

#include "../tcc/OutputPortElements.tcc"
