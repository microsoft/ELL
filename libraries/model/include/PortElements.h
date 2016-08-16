////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PortElements.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "OutputPort.h"

// utilities
#include "Exception.h"
#include "ISerializable.h"

// stl
#include <vector>
#include <cassert>
#include <algorithm>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Represents a contiguous set of values from an output port </summary>
    class PortRange : public utilities::ISerializable
    {
    public:
        PortRange() = default;

        /// <summary> Creates a PortRange representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        PortRange(const OutputPortBase& port);

        /// <summary> Creates a PortRange representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortRange(const OutputPortBase& port, size_t index);

        /// <summary> Creates a PortRange representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        PortRange(const OutputPortBase& port, size_t startIndex, size_t numValues);

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

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PortRange"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

        /// <summary> Equality operatior. </summary>
        ///
        /// <returns> true if this range is equivalent to other. </returns>
        bool operator==(const PortRange& other) const;

    private:
        const OutputPortBase* _referencedPort = nullptr;
        size_t _startIndex = 0;
        size_t _numValues = 0;
        bool _isFixedSize = true;
    };

    /// <summary> Represents a set of values from one or more output ports </summary>
    class PortElementsUntyped : public utilities::ISerializable
    {
    public:
        /// <summary> Creates an PortElementsUntyped representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        PortElementsUntyped(const OutputPortBase& port);

        /// <summary> Creates an PortElementsUntyped representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElementsUntyped(const OutputPortBase& port, size_t startIndex);

        /// <summary> Creates an PortElementsUntyped representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        PortElementsUntyped(const OutputPortBase& port, size_t startIndex, size_t numValues);

        /// <summary> Creates an PortElementsUntyped from a PortRange </summary>
        ///
        /// <param name="range"> The range to get values from </param>
        PortElementsUntyped(const PortRange& range);

        /// <summary> Creates an PortElementsUntyped from a set of PortRange </summary>
        ///
        /// <param name="range"> The ranges to get values from </param>
        PortElementsUntyped(const std::vector<PortRange>& ranges);

        virtual ~PortElementsUntyped() = default;

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; }

        /// <summary> The number of ranges in this list </summary>
        ///
        /// <returns> The number of ranges in this list </returns>
        size_t NumRanges() const { return _ranges.size(); }

        /// <summary> An STL-type iterator pointing to the beginning of the list of ranges </summary>
        std::vector<PortRange>::const_iterator begin() const { return _ranges.cbegin(); }

        /// <summary> An STL-type iterator pointing to the end of the list of ranges </summary>
        std::vector<PortRange>::const_iterator end() const { return _ranges.cend(); }

        PortRange GetRange(size_t index) const { return _ranges[index]; }

        // std::unordered_set<Node*> usedNodes;
        // for(const auto& range: elements)
        // {
        //     usedNodes.insert(range.ReferencedPort()->GetNode());
        // }
        // auto nodes = std::vector<Node*>(usedNodes.begin(), usedNodes.end());


        /// <summary> Gets an element in the elements. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> The specified element. </returns>
        PortRange GetElement(size_t index) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PortElementsUntyped"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

    protected:
        PortElementsUntyped(){};
        void ComputeSize();
        void AddRange(const PortRange& range);

    private:
        std::vector<PortRange> _ranges;
        // TODO: keep set of nodes (?)
        size_t _size = 0;
    };

    /// <summary> Represents a statically-typed set of values from one or more output ports </summary>
    template <typename ValueType>
    class PortElements : public PortElementsUntyped
    {
    public:
        PortElements() = default;

        /// <summary> Creates a PortElements representing all the values from a given port </summary>
        ///
        /// <param name="port"> The port to take values from </param>
        PortElements(const OutputPort<ValueType>& port);

        /// <summary> Creates a PortElements representing a single value from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElements(const OutputPort<ValueType>& port, size_t startIndex);

        /// <summary> Creates a PortElements representing a range of values from a given port </summary>
        ///
        /// <param name="port"> The port to take a value from </param>
        /// <param name="startIndex"> The index of the first value to take </param>
        /// <param name="numValues"> The number of values to take </param>
        PortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

        /// <summary> Creates a PortElements by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The list of groups to concantenate together </param>
        PortElements(const std::initializer_list<PortElements<ValueType>>& groups);

        /// <summary> Creates a PortElements by concatenating a set of them together </summary>
        ///
        /// <param name="groups"> The vector of groups to concantenate together </param>
        PortElements(const std::vector<PortElements<ValueType>>& groups);
    
        /// <summary> Creates a PortElements representing a single value from a given PortElements </summary>
        ///
        /// <param name="elements"> The PortElements to take a value from </param>
        /// <param name="index"> The index of the value </param>
        PortElements(const PortElements<ValueType>& elements, size_t index);

        /// <summary> Creates a PortElements representing a single value from a given PortElements </summary>
        ///
        /// <param name="elements"> The PortElements to take a value from </param>
        /// <param name="startIndex"> The index of the first value to use </param>
        /// <param name="numValues"> The number of values to take </param>
        PortElements(const PortElements<ValueType>& elements, size_t startIndex, size_t numValues);

        /// <summary> Appends a set of elements to this set of elements. </summary>
        ///
        /// <param name="other"> The PortElements to append to this one. </param>
        void Append(const PortElements<ValueType>& other);
    };

    //
    // Helper functions
    //

    /// <summary> Creates a PortElements representing all the values from a given port </summary>
    ///
    /// <param name="port"> The port to take values from </param>
    /// <returns> The composite PortElements </returns>
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port);

    /// <summary> Creates a PortElements representing a single value from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="index"> The index of the value </param>
    /// <returns> The composite PortElements </returns>
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port, size_t startIndex);

    /// <summary> Creates a PortElements representing a range of values from a given port </summary>
    ///
    /// <param name="port"> The port to take a value from </param>
    /// <param name="startIndex"> The index of the first value to take </param>
    /// <param name="numValues"> The number of values to take </param>
    /// <returns> The composite PortElements </returns>
    template <typename ValueType>
    PortElements<ValueType> MakePortElements(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues);

    /// <summary> Creates a PortElements by concatenating together one or more PortElementss
    ///
    /// <param name="ref"> The PortElementss to concatenate together </param>
    /// <returns> The composite PortElements </returns>
    template <typename RefType, typename... Refs>
    RefType Concat(const RefType& ref1, Refs&&... refs);
}

// custom specialization of std::hash so we can keep PortRanges in containers that require hashable types
namespace std
{
    /// <summary> Implements a hash function for the PortRange class, so that it can be used with associative containers (maps, sets, and the like). </summary>
    template <>
    class hash<model::PortRange>
    {
    public:
        typedef model::PortRange argument_type;
        typedef std::size_t result_type;

        /// <summary> Computes a hash of the input value. </summary>
        ///
        /// <returns> A hash value for the given input. </returns>
        result_type operator()(argument_type const& id) const;
    };
}

#include "../tcc/PortElements.tcc"
