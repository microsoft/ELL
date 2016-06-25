////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortRef.h (model)
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
    /// <summary> Represents a reference to a single element in another node's output </summary>
    class OutputPortElementRef
    {
    public:
        /// <summary> Creates an OutputPortElementRef representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortElementRef(const Port* port, size_t index);

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
        const Port* ReferencedPort() const { return _referencedPort; }

    private:
        const Port* _referencedPort;
        size_t _index;
    };
    
    /// <summary> Represents a contiguous set of values from an output port </summary>
    class OutputPortRange
    {
    public:
        /// <summary> Creates an OutputPortRange representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortRange(const Port& port);

        /// <summary> Creates an OutputPortRange representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortRange(const Port& port, size_t index);

        /// <summary> Creates an OutputPortRange representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortRange(const Port& port, size_t startIndex, size_t numValues);

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
        const Port* ReferencedPort() const { return _referencedPort; }

    private:
        const Port* _referencedPort;
        size_t _startIndex;
        size_t _numValues;
        bool _isFixedSize;
    };

    /// <summary> Represents a set of values from one or more output ports </summary>
    class OutputPortRefUntyped
    {
    public:
        /// <summary> Creates an OutputPortRefUntyped representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortRefUntyped(const Port& port);

        /// <summary> Creates an OutputPortRefUntyped representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortRefUntyped(const Port& port, size_t startIndex);

        /// <summary> Creates an OutputPortRefUntyped representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortRefUntyped(const Port& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an OutputPortRefUntyped from an OutputPortRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        OutputPortRefUntyped(const OutputPortRange& range);

        /// <summary> Creates an OutputPortRefUntyped from a set of OutputPortRange </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        OutputPortRefUntyped(const std::vector<OutputPortRange>& ranges);

        /// <summary> Returns a single-element range for the element at the given index </summary>
        ///
        /// <param name="index"> The index of the element </param>
        /// <returns> A single-element range for the element </returns>
        OutputPortRange GetOutputPortElementRef(size_t index) const;

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<OutputPortRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<OutputPortRange>::const_iterator end() const { return _ranges.cend(); }

    protected:
        OutputPortRefUntyped(){};
        void ComputeSize();
        void AddRange(const OutputPortRange& range);
    
    private:
        std::vector<OutputPortRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class OutputPortRef : public OutputPortRefUntyped
    {
    public:
        /// <summary> Creates a OutputPortRef representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        OutputPortRef(const OutputPort<ValueType>& port);

        /// <summary> Creates a OutputPortRef representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputPortRef(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a OutputPortRef representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputPortRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a OutputPortRef by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputPortRef(const std::initializer_list<OutputPortRef<ValueType>>& groups);

        /// <summary> Creates a OutputPortRef by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputPortRef(const std::vector<OutputPortRef<ValueType>>& groups);
    };

    //
    // Helper functions
    //

    /// <summary> Creates a OutputPortRef representing all the values from a given port </summary>
    ///
    /// <param name="port"> The port to take values from </param>
    /// <returns> The composite OutputPortRef </returns>
    template <typename ValueType>
    OutputPortRef<ValueType> MakeRef(const OutputPort<ValueType>& port);

    /// <summary> Creates a OutputPortRef representing a single value from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="index"> The index of the value </param>
    /// <returns> The composite OutputPortRef </returns>
    template <typename ValueType>
    OutputPortRef<ValueType> MakeRef(const OutputPort<ValueType>& port, size_t startIndex);

    /// <summary> Creates a OutputPortRef representing a range of values from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="startIndex"> The index of the first value to take </param>
    /// <param name="numValues"> The number of values to take </param>
    /// <returns> The composite OutputPortRef </returns>
    template <typename ValueType>
    OutputPortRef<ValueType> MakeRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

    /// <summary> Creates a OutputPortRef by concatenating together one or more OutputPortRefs
    ///
    /// <param name="ref"> The OutputPortRefs to concatenate together </param>
    /// <returns> The composite OutputPortRef </returns>
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs);
}

#include "../tcc/OutputPortRef.tcc"
