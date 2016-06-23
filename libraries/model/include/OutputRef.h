////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputRef.h (model)
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

        /// <summary> The index of the first element this range refers to </summary>
        ///
        /// <returns> The index of the first element this range refers to </returns>
        size_t StartIndex() const;

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
    class UntypedOutputRef
    {
    public:
        /// <summary> Creates an UntypedOutputRef representing all the values from a given port </summary>
        UntypedOutputRef(const Port& port);

        /// <summary> Creates an UntypedOutputRef representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        UntypedOutputRef(const Port& port, size_t startIndex);

        /// <summary> Creates an UntypedOutputRef representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        UntypedOutputRef(const Port& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an UntypedOutputRef from an OutputRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        UntypedOutputRef(const OutputRange& range);

        /// <summary> Creates an UntypedOutputRef from a set of OutputRange </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        UntypedOutputRef(const std::vector<OutputRange>& ranges);

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<OutputRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<OutputRange>::const_iterator end() const { return _ranges.cend(); }

    protected:
        UntypedOutputRef(){};
        void ComputeSize();

        std::vector<OutputRange> _ranges;
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class OutputRef : public UntypedOutputRef
    {
    public:
        /// <summary> Creates a OutputRef representing all the values from a given port </summary>
        OutputRef(const OutputPort<ValueType>& port);

        /// <summary> Creates a OutputRef representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        OutputRef(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a OutputRef representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        OutputRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a OutputRef by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputRef(const std::initializer_list<OutputRef<ValueType>>& groups);

        /// <summary> Creates a OutputRef by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        OutputRef(const std::vector<OutputRef<ValueType>>& groups);
    };

    //
    // Helper functions
    //

    // MakeRef
    template <typename ValueType>
    OutputRef<ValueType> MakeRef(const OutputPort<ValueType>& port)
    {
        return OutputRef<ValueType>(port);
    }

    template <typename ValueType>
    OutputRef<ValueType> MakeRef(const OutputPort<ValueType>& port, size_t startIndex)
    {
        return OutputRef<ValueType>(port, startIndex);
    }

    template <typename ValueType>
    OutputRef<ValueType> MakeRef(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues)
    {
        return OutputRef<ValueType>(port, startIndex, numValues);
    }

    // Concat
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs)
    {
        return RefType({ ref1, refs... });
    }
}

#include "../tcc/OutputRef.tcc"
